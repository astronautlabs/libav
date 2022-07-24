#include "codec-context.h"
#include "codec.h"
#include "../common.h"
#include "../avutil/class.h"
#include "../avutil/dict.h"
#include "../avcodec/packet.h"
#include "../avutil/frame.h"

#include <iostream>

NAVCodecContext::NAVCodecContext(const Napi::CallbackInfo& info):
    NAVResource(info)
{
    if (ConstructFromHandle(info))
        return;
    
    auto codec = NAVCodec::Unwrap(info[0].As<Napi::Object>());
    auto handle = avcodec_alloc_context3(codec->GetHandle());
    SetHandle(handle);
}

void NAVCodecContext::ThreadMain() {
    std::unique_lock<std::mutex> lock(mutex);
    AVCodecContext *context = GetHandle();
    bool isEncoder = av_codec_is_encoder(context->codec);
    bool isDecoder = av_codec_is_decoder(context->codec);
    lock.unlock();
    
    while (running) {
        bool fed = FeedToCodec();

        if (isEncoder)
            PullFromEncoder(context);
        else if (isDecoder)
            PullFromDecoder(context);

        if (!fed)
            WaitForWork();
    }
}

void NAVCodecContext::WaitForWork() {
    std::unique_lock<std::mutex> lock(mutex);

    if (inQueue.size() > 0)
        return;

    threadWake.wait(lock);
}

AVFrame *NAVCodecContext::GetPoolFrame() {
    std::unique_lock<std::mutex> lock(mutex);

    if (framePool.size() == 0)
        framePool.push(av_frame_alloc());
    
    auto frame = framePool.front();
    framePool.pop();
    framePoolHeight += 1;

    return frame;
}

void NAVCodecContext::FreePoolFrame(AVFrame *frame) {
    framePool.push(frame);
    framePoolHeight -= 1;

    // Burn a single frame beyond the current height
    if (framePool.size() > framePoolHeight + 4) {
        auto burnt = framePool.front();
        framePool.pop();
        av_frame_free(&burnt);
    }
}

AVPacket *NAVCodecContext::GetPoolPacket() {
    std::unique_lock<std::mutex> lock(mutex);

    if (packetPool.size() == 0)
        packetPool.push(av_packet_alloc());
    
    auto frame = packetPool.front();
    packetPool.pop();
    packetPoolHeight += 1;

    return frame;
}

void NAVCodecContext::FreePoolPacket(AVPacket *frame) {
    packetPool.push(frame);
    packetPoolHeight -= 1;

    // Burn a single frame beyond the current height
    if (packetPool.size() > packetPoolHeight + 4) {
        auto burnt = packetPool.front();
        packetPool.pop();
        av_packet_free(&burnt);
    }
}

void NAVCodecContext::SendError(std::string code, std::string message) {

    if (!this->onErrorValid) {
        std::cerr << "[AVCodecContext thread]: Error [discarded]: " << code << ": " << message << std::endl;
        return;
    }

    std::cerr << "[AVCodecContext thread]: Error: " << code << ": " << message << std::endl;

    onErrorTSFN.BlockingCall([=](Napi::Env env, Napi::Function jsCallback) {
        Napi::Object error = Napi::Object::New(env);
        error.Set("code", code);
        error.Set("message", message);
        jsCallback.Call({ error });
    });
}

void NAVCodecContext::ThreadLog(std::string message) {
    if (!threadTracing)
        return;

    std::cout << "[AVCodecContext thread] " << message << std::endl;
}

bool NAVCodecContext::FeedToCodec() {
    std::unique_lock<std::mutex> lock(mutex);
    auto handle = GetHandle();
    std::deque<WorkItem> localQueue;

    localQueue = inQueue;
    inQueue.clear();
    lock.unlock();

    if (localQueue.size() == 0)
        return false;
    
    bool fed = false;

    while (running && localQueue.size() > 0) {
        auto workItem = localQueue.front();
        int result;

        if (workItem.frame) {
            result = avcodec_send_frame(handle, workItem.frame);
        } else if (workItem.packet) {
            result = avcodec_send_packet(handle, workItem.packet);
        }

        if (result == AVERROR(EAGAIN))
            break;

        if (result < 0) {
            SendError("averror:" + std::to_string(result), "An error occurred during avcodec_send. Discarding queued item.");
            localQueue.pop_front();
            break;
        }

        fed = true;
        localQueue.pop_front();
    }

    // Return unprocessed items back to the queue
    if (localQueue.size() > 0) {
        std::unique_lock<std::mutex> lock(mutex);

        while (localQueue.size() > 0) {
            inQueue.push_front(localQueue.back());
            localQueue.pop_back();
        }
    }

    return fed;
}

bool NAVCodecContext::PullFromDecoder(AVCodecContext *context) {
    while (running) {
        if (!onFrameValid)
            break;
        
        auto frame = GetPoolFrame();
        int result = avcodec_receive_frame(context, frame);

        if (AVERROR(result) == EAGAIN) {
            FreePoolFrame(frame);
            break;
        }

        if (result < 0) {
            SendError("averror:" + result, "An error occurred during avcodec_receive_frame");
            break;
        }

        // Send to the main thread

        onFrameTSFN.BlockingCall([=](Napi::Env env, Napi::Function jsCallback) {
            jsCallback.Call({ NAVFrame::FromHandleWrapped(env, frame, true) });
        });
    }

    return true;
}

bool NAVCodecContext::PullFromEncoder(AVCodecContext *context) {
    ThreadLog("Pulling from encoder...");

    while (running) {
        if (!onPacketValid)
            break;

        auto packet = GetPoolPacket();
        int result = avcodec_receive_packet(context, packet);

        if (AVERROR(result) == EAGAIN || result == AVERROR_EOF) {
            ThreadLog("Encoder is starved.");
            FreePoolPacket(packet);
            break;
        }

        if (result < 0) {
            SendError("averror:" + result, "An error occurred during avcodec_receive_packet");
            break;
        }

        // Send to the main thread
        ThreadLog("** Received packet!");
        onPacketTSFN.BlockingCall([=](Napi::Env env, Napi::Function jsCallback) {
            jsCallback.Call({ NAVPacket::FromHandleWrapped(env, packet, true) });
        });
    }

    return true;
}

void NAVCodecContext::Free() {
    running = false;
    auto handle = GetHandle();
    avcodec_free_context(&handle);

    // End the thread
    threadWake.notify_one();
    thread->join();
    
    SetHandle(handle);
}

Napi::Value NAVCodecContext::Open(const Napi::CallbackInfo& info) {
    if (opened) {
        Napi::Error::New(info.Env(), "This codec context is already opened. You cannot reuse a codec context.").ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    NAVDictionary *nOptions = nullptr;
    AVDictionary *options = nullptr;
    if (info.Length() > 1) {
        nOptions = NAVDictionary::Unwrap(info[1].As<Napi::Object>());
        options = nOptions->GetHandle();
    }

    auto handle = GetHandle();
    int result = avcodec_open2(handle, handle->codec, &options);

    if (result < 0)
        return nlav_throw(info.Env(), result, "avcodec_open2");

    opened = true;
    
    if (nOptions)
        nOptions->SetHandle(options);

    thread = new std::thread([](NAVCodecContext *context) { context->ThreadMain(); }, this);

    return info.Env().Undefined();
}

Napi::Value NAVCodecContext::SendPacket(const Napi::CallbackInfo& info) {
    auto packet = NAVPacket::Unwrap(info[0].As<Napi::Object>());
    auto result = avcodec_send_packet(GetHandle(), packet->GetHandle());
    if (result < 0)
        return nlav_throw(info.Env(), result, "avcodec_send_packet");
    return info.Env().Undefined();
}

Napi::Value NAVCodecContext::ReceiveFrame(const Napi::CallbackInfo& info) {
    NAVFrame *frame;

    if (info.Length() > 0)
        frame = NAVFrame::Unwrap(info[0].As<Napi::Object>());
    else
        frame = LibAvAddon::Construct<NAVFrame>(info.Env());
    
    int result = avcodec_receive_frame(GetHandle(), frame->GetHandle());
    if (result < 0)
        return nlav_throw(info.Env(), result, "avcodec_receive_frame");

    return frame->Value();
}

Napi::Value NAVCodecContext::SendFrame(const Napi::CallbackInfo& info) {
    NAVFrame *frame = NAVFrame::Unwrap(info[0].As<Napi::Object>());
    std::unique_lock<std::mutex> lock(mutex);
    WorkItem item;

    item.frame = frame->GetHandle();
    inQueue.push_back(item);

    threadWake.notify_one();
    return Napi::Boolean::New(info.Env(), true);
}

Napi::Value NAVCodecContext::ReceivePacket(const Napi::CallbackInfo& info) {
    NAVPacket *packet;

    if (info.Length() > 0)
        packet = NAVPacket::Unwrap(info[0].As<Napi::Object>());
    else
        packet = LibAvAddon::Construct<NAVPacket>(info.Env());
    
    int result = avcodec_receive_packet(GetHandle(), packet->GetHandle());
    if (result < 0)
        return nlav_throw(info.Env(), result, "avcodec_receive_packet");

    return packet->Value();
}

Napi::Value NAVCodecContext::GetOnFrame(const Napi::CallbackInfo& info) {
    return onFrame.Value();
}

void NAVCodecContext::SetOnFrame(const Napi::CallbackInfo& info, const Napi::Value &value) {
    auto func = info[0].As<Napi::Function>();

    if (func != info.Env().Null() && func != info.Env().Undefined()) {
        onFrame = Napi::Persistent(func);
        onFrameTSFN = Napi::ThreadSafeFunction::New(info.Env(), func, "AVContext#onFrame", 0, 1);
        onFrameValid = true;
        threadWake.notify_one();
    } else {
        if (!onFrame.IsEmpty())
            onFrame.Unref();
        onFrame = Napi::FunctionReference();
        onFrameTSFN.Release();
        onFrameValid = false;
    }
}

Napi::Value NAVCodecContext::GetOnPacket(const Napi::CallbackInfo& info) {
    return onPacket.Value();
}

void NAVCodecContext::SetOnPacket(const Napi::CallbackInfo& info, const Napi::Value &value) {
    auto func = info[0].As<Napi::Function>();

    if (func != info.Env().Null() && func != info.Env().Undefined()) {
        onPacket = Napi::Persistent(func);
        onPacketTSFN = Napi::ThreadSafeFunction::New(info.Env(), func, "AVContext#onPacket", 0, 1);
        onPacketValid = true;
        threadWake.notify_one();
    } else {
        if (!onPacket.IsEmpty())
            onPacket.Unref();
        onPacket = Napi::FunctionReference();
        onPacketTSFN.Release();
        onPacketValid = false;
    }
}

Napi::Value NAVCodecContext::GetOnError(const Napi::CallbackInfo& info) {
    return onError.Value();
}

void NAVCodecContext::SetOnError(const Napi::CallbackInfo& info, const Napi::Value &value) {
    auto func = info[0].As<Napi::Function>();

    if (func != info.Env().Null() && func != info.Env().Undefined()) {
        onError = Napi::Persistent(func);
        onErrorTSFN = Napi::ThreadSafeFunction::New(info.Env(), func, "AVContext#onError", 0, 1);
        onErrorValid = true;
    } else {
        if (!onError.IsEmpty())
            onError.Unref();
        onError = Napi::FunctionReference();
        onErrorTSFN.Release();
        onErrorValid = false;
    }
}


Napi::Value NAVCodecContext::GetClass(const Napi::CallbackInfo& info) {
    return NAVClass::FromHandleWrapped(info.Env(), (AVClass*)GetHandle()->av_class, false);
}

Napi::Value NAVCodecContext::GetCodecType(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->codec_type);
}

Napi::Value NAVCodecContext::GetCodecId(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->codec_id);
}

Napi::Value NAVCodecContext::GetCodecTag(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->codec_tag);
}

void NAVCodecContext::SetBitRate(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->bit_rate = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetBitRate(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->bit_rate);
}

void NAVCodecContext::SetBitRateTolerance(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->bit_rate_tolerance = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetBitRateTolerance(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->bit_rate_tolerance);
}

void NAVCodecContext::SetGlobalQuality(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->global_quality = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetGlobalQuality(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->global_quality);
}

void NAVCodecContext::SetCompressionLevel(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->compression_level = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetCompressionLevel(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->compression_level);
}

void NAVCodecContext::SetFlags(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->flags = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetFlags(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->flags);
}

void NAVCodecContext::SetFlags2(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->flags2 = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetFlags2(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->flags2);
}

void NAVCodecContext::SetTimeBase(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->time_base = FromNRational(info.Env(), value);
}

Napi::Value NAVCodecContext::GetTimeBase(const Napi::CallbackInfo& info) {
    return NRational(info.Env(), GetHandle()->time_base);
}

void NAVCodecContext::SetTicksPerFrame(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->ticks_per_frame = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetTicksPerFrame(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->ticks_per_frame);
}

void NAVCodecContext::SetDelay(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->delay = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetDelay(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->delay);
}

void NAVCodecContext::SetWidth(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->width = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetWidth(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->width);
}

void NAVCodecContext::SetHeight(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->height = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetHeight(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->height);
}

void NAVCodecContext::SetCodedWidth(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->coded_width = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetCodedWidth(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->coded_width);
}

void NAVCodecContext::SetCodedHeight(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->coded_height = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetCodedHeight(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->coded_height);
}

void NAVCodecContext::SetGopSize(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->gop_size = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetGopSize(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->gop_size);
}

void NAVCodecContext::SetPixelFormat(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->pix_fmt = (AVPixelFormat)value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetPixelFormat(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->pix_fmt);
}

void NAVCodecContext::SetMaxBFrames(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->max_b_frames = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetMaxBFrames(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->max_b_frames);
}

void NAVCodecContext::SetBQuantizationFactor(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->b_quant_factor = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetBQuantizationFactor(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->b_quant_factor);
}

void NAVCodecContext::SetBQuantizationOffset(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->b_quant_offset = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetBQuantizationOffset(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->b_quant_offset);
}

void NAVCodecContext::SetHasBFrames(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->has_b_frames = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetHasBFrames(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), GetHandle()->has_b_frames);
}

void NAVCodecContext::SetIQuantizationFactor(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->i_quant_factor = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetIQuantizationFactor(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->i_quant_factor);
}

void NAVCodecContext::SetIQuantizationOffset(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->i_quant_offset = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetIQuantizationOffset(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->i_quant_offset);
}

void NAVCodecContext::SetLuminanceMasking(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->lumi_masking = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetLuminanceMasking(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->lumi_masking);
}

void NAVCodecContext::SetTemporalComplexityMasking(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->temporal_cplx_masking = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetTemporalComplexityMasking(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->temporal_cplx_masking);
}

void NAVCodecContext::SetSpatialComplexityMasking(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->spatial_cplx_masking = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetSpatialComplexityMasking(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->spatial_cplx_masking);
}

void NAVCodecContext::SetPMasking(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->p_masking = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetPMasking(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->p_masking);
}

void NAVCodecContext::SetDarkMasking(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->dark_masking = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetDarkMasking(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->dark_masking);
}

void NAVCodecContext::SetSampleAspectRatio(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->sample_aspect_ratio = FromNRational(info.Env(), value);
}

Napi::Value NAVCodecContext::GetSampleAspectRatio(const Napi::CallbackInfo& info) {
    return NRational(info.Env(), GetHandle()->sample_aspect_ratio);
}

void NAVCodecContext::SetMotionEstimationComparisonFunction(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->me_cmp = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetMotionEstimationComparisonFunction(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->me_cmp);
}

void NAVCodecContext::SetSubpixelMotionEstimationComparisonFunction(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->me_sub_cmp = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetSubpixelMotionEstimationComparisonFunction(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->me_sub_cmp);
}

void NAVCodecContext::SetMacroblockComparisonFunction(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->mb_cmp = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetMacroblockComparisonFunction(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->mb_cmp);
}

void NAVCodecContext::SetInterlacedDCTComparisonFunction(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->ildct_cmp = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetInterlacedDCTComparisonFunction(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->ildct_cmp);
}

void NAVCodecContext::SetDiamondSize(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->dia_size = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetDiamondSize(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->dia_size);
}

void NAVCodecContext::SetLastPredictorCount(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->last_predictor_count = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetLastPredictorCount(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->last_predictor_count);
}

void NAVCodecContext::SetMotionEstimationPrepassComparisonFunction(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->me_pre_cmp = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetMotionEstimationPrepassComparisonFunction(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->me_pre_cmp);
}

void NAVCodecContext::SetMotionEstimationPrepassDiamondSize(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->pre_dia_size = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetMotionEstimationPrepassDiamondSize(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->pre_dia_size);
}

void NAVCodecContext::SetMotionEstimationSubpelQuality(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->me_subpel_quality = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetMotionEstimationSubpelQuality(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->me_subpel_quality);
}

void NAVCodecContext::SetMotionEstimationRange(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->me_range = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetMotionEstimationRange(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->me_range);
}

void NAVCodecContext::SetSliceFlags(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->slice_flags = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetSliceFlags(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->slice_flags);
}

void NAVCodecContext::SetMacroblockDecisionMode(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->mb_decision = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetMacroblockDecisionMode(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->mb_decision);
}

void NAVCodecContext::SetIntraDCPrecision(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->intra_dc_precision = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetIntraDCPrecision(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->intra_dc_precision);
}


void NAVCodecContext::SetSkipTop(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->skip_top = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetSkipTop(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->skip_top);
}

void NAVCodecContext::SetSkipBottom(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->skip_bottom = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetSkipBottom(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->skip_bottom);
}

void NAVCodecContext::SetMinMBLagrangeMultiplier(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->mb_lmin = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetMinMBLagrangeMultiplier(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->mb_lmin);
}

void NAVCodecContext::SetMaxLBLagrangeMultiplier(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->mb_lmax = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetMaxLBLagrangeMultiplier(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->mb_lmax);
}

void NAVCodecContext::SetBidirectionalRefine(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->bidir_refine = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetBidirectionalRefine(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->bidir_refine);
}

void NAVCodecContext::SetMinGopSize(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->keyint_min = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetMinGopSize(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->keyint_min);
}

void NAVCodecContext::SetReferenceFrameCount(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->refs = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetReferenceFrameCount(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->refs);
}

void NAVCodecContext::SetMv0Threshold(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->mv0_threshold = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetMv0Threshold(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->mv0_threshold);
}

void NAVCodecContext::SetColorPrimaries(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->color_primaries = (AVColorPrimaries)value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetColorPrimaries(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->color_primaries);
}

void NAVCodecContext::SetColorTrc(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->color_trc = (AVColorTransferCharacteristic)value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetColorTrc(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->color_trc);
}

void NAVCodecContext::SetColorSpace(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->colorspace = (AVColorSpace)value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetColorSpace(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->colorspace);
}

void NAVCodecContext::SetColorRange(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->color_range = (AVColorRange)value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetColorRange(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->color_range);
}

void NAVCodecContext::SetChromeSampleLocation(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->chroma_sample_location = (AVChromaLocation)value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetChromeSampleLocation(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->chroma_sample_location);
}

void NAVCodecContext::SetSliceCount(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->slice_count = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetSliceCount(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->slice_count);
}

void NAVCodecContext::SetFieldOrder(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->field_order = (AVFieldOrder)value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetFieldOrder(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->field_order);
}

void NAVCodecContext::SetSampleRate(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->sample_rate = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetSampleRate(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->sample_rate);
}

void NAVCodecContext::SetSampleFormat(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->sample_fmt = (AVSampleFormat)value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetSampleFormat(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->sample_fmt);
}

void NAVCodecContext::SetFrameSize(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->frame_size = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetFrameSize(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->frame_size);
}

void NAVCodecContext::SetFrameNumber(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->frame_number = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetFrameNumber(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->frame_number);
}

void NAVCodecContext::SetBlockAlignment(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->block_align = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetBlockAlignment(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->block_align);
}

void NAVCodecContext::SetCutoff(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->cutoff = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetCutoff(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->cutoff);
}

void NAVCodecContext::SetAudioServiceType(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->audio_service_type = (AVAudioServiceType)value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetAudioServiceType(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->audio_service_type);
}

void NAVCodecContext::SetRequestSampleFormat(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->request_sample_fmt = (AVSampleFormat)value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetRequestSampleFormat(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->request_sample_fmt);
}

void NAVCodecContext::SetQuantizerCompression(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->qcompress = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetQuantizerCompression(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->qcompress);
}

void NAVCodecContext::SetQuantizerBlur(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->qblur = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetQuantizerBlur(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->qblur);
}

void NAVCodecContext::SetMinQuantizer(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->qmin = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetMinQuantizer(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->qmin);
}

void NAVCodecContext::SetMaxQuantizer(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->qmax = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetMaxQuantizer(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->qmax);
}

void NAVCodecContext::SetMaxQuantizerDifference(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->max_qdiff = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetMaxQuantizerDifference(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->max_qdiff);
}

void NAVCodecContext::SetRateControlBufferSize(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->rc_buffer_size = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetRateControlBufferSize(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->rc_buffer_size);
}

void NAVCodecContext::SetRateControlOverrideCount(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->rc_override_count = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetRateControlOverrideCount(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->rc_override_count);
}

void NAVCodecContext::SetRateControlMaxBitrate(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->rc_max_rate = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetRateControlMaxBitrate(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->rc_max_rate);
}

void NAVCodecContext::SetRateControlMinBitrate(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->rc_min_rate = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetRateControlMinBitrate(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->rc_min_rate);
}

void NAVCodecContext::SetRateControlMaxAvailableVbvUse(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->rc_max_available_vbv_use = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetRateControlMaxAvailableVbvUse(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->rc_max_available_vbv_use);
}

void NAVCodecContext::SetRateControlMinVbvOverflowUse(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->rc_min_vbv_overflow_use = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetRateControlMinVbvOverflowUse(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->rc_min_vbv_overflow_use);
}

void NAVCodecContext::SetRateControlInitialBufferOccupancy(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->rc_initial_buffer_occupancy = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetRateControlInitialBufferOccupancy(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->rc_initial_buffer_occupancy);
}

void NAVCodecContext::SetTrellis(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->trellis = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetTrellis(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->trellis);
}

void NAVCodecContext::SetWorkaroundBugs(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->workaround_bugs = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetWorkaroundBugs(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->workaround_bugs);
}

void NAVCodecContext::SetErrorRecognitionFlags(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->err_recognition = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVCodecContext::GetErrorRecognitionFlags(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->err_recognition);
}


Napi::Value NAVCodecContext::GetChannelLayout(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->channel_layout);
}

void NAVCodecContext::SetChannelLayout(const Napi::CallbackInfo& info, const Napi::Value& value) {
    auto layout = info[0].As<Napi::Number>().Int32Value();
    GetHandle()->channel_layout = layout;
}