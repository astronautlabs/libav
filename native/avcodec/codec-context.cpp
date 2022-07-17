#include "codec-context.h"
#include "codec.h"
#include "../common.h"
#include "../avutil/class.h"
#include "../avutil/dict.h"
#include "../avcodec/packet.h"
#include "../avutil/frame.h"

NAVCodecContext::NAVCodecContext(const Napi::CallbackInfo& info):
    NAVResource(info)
{
    if (ConstructFromHandle(info))
        return;
    
    auto codec = NAVCodec::Unwrap(info[0].As<Napi::Object>());
    auto handle = avcodec_alloc_context3(codec->GetHandle());
    NAVDictionary *nOptions = nullptr;
    AVDictionary *options = nullptr;
    if (info.Length() > 1) {
        nOptions = NAVDictionary::Unwrap(info[1].As<Napi::Object>());
        options = nOptions->GetHandle();
    }

    avcodec_open2(handle, codec->GetHandle(), &options);

    if (nOptions)
        nOptions->SetHandle(options);
    
    SetHandle(handle);

    thread = new std::thread([](NAVCodecContext *context) { context->ThreadMain(); }, this);
}

void NAVCodecContext::ThreadMain() {
    std::unique_lock<std::mutex> lock(mutex);
    AVCodecContext *context = GetHandle();
    bool isEncoder = av_codec_is_encoder(context->codec);
    bool isDecoder = av_codec_is_decoder(context->codec);

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


bool NAVCodecContext::FeedToCodec() {
    std::unique_lock<std::mutex> lock(mutex);
    auto handle = GetHandle();
    std::deque<WorkItem> localQueue;

    inQueue = localQueue;
    inQueue.empty();
    lock.unlock();

    if (localQueue.size() == 0)
        return false;
    
    while (running && localQueue.size() > 0) {
        auto workItem = inQueue.front();
        int result;

        if (workItem.frame) {
            avcodec_send_frame(handle, workItem.frame);
        } else if (workItem.packet) {
            avcodec_send_packet(handle, workItem.packet);
        }

        if (result == AVERROR(EAGAIN))
            break;

        assert(result == 0); // TODO: convey this back to main thread
        inQueue.pop_front();
    }

    // Return unprocessed items back to the queue
    if (localQueue.size() > 0) {
        std::unique_lock<std::mutex> lock(mutex);

        while (localQueue.size() > 0) {
            inQueue.push_front(localQueue.back());
            localQueue.pop_back();
        }
    }
}

bool NAVCodecContext::PullFromDecoder(AVCodecContext *context) {
    while (running) {
        auto frame = GetPoolFrame();
        int result = avcodec_receive_frame(context, frame);

        if (AVERROR(result) == EAGAIN) {
            FreePoolFrame(frame);
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
    while (running) {
        auto packet = GetPoolPacket();
        int result = avcodec_receive_packet(context, packet);

        if (AVERROR(result) == EAGAIN) {
            FreePoolPacket(packet);
            break;
        }

        // Send to the main thread
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
    SetHandle(handle);
}

Napi::Value NAVCodecContext::SendPacket(const Napi::CallbackInfo& info) {
    auto packet = NAVPacket::Unwrap(info[0].As<Napi::Object>());
    auto result = avcodec_send_packet(GetHandle(), packet->GetHandle());
    if (result < 0)
        return nlav_throw(info.Env(), result);
}

Napi::Value NAVCodecContext::ReceiveFrame(const Napi::CallbackInfo& info) {
    NAVFrame *frame;

    if (info.Length() > 0)
        frame = NAVFrame::Unwrap(info[0].As<Napi::Object>());
    else
        frame = LibAvAddon::Construct<NAVFrame>(info.Env());
    
    int result = avcodec_receive_frame(GetHandle(), frame->GetHandle());
    if (result < 0)
        return nlav_throw(info.Env(), result);

    return frame->Value();
}

Napi::Value NAVCodecContext::SendFrame(const Napi::CallbackInfo& info) {
    NAVFrame *frame = NAVFrame::Unwrap(info[0].As<Napi::Object>());
    int result = avcodec_send_frame(GetHandle(), frame->GetHandle());
    if (result < 0)
        return nlav_throw(info.Env(), result);

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
        return nlav_throw(info.Env(), result);

    return packet->Value();
}

Napi::Value NAVCodecContext::GetOnFrame(const Napi::CallbackInfo& info) {
    return onFrame.Value();
}

void NAVCodecContext::SetOnFrame(const Napi::CallbackInfo& info, const Napi::Value &value) {
    auto func = info[0].As<Napi::Function>();
    onFrame = Napi::Persistent(func);
    onFrameTSFN = Napi::ThreadSafeFunction::New(info.Env(), func, "AVContext#onFrame", 0, 1);
}

Napi::Value NAVCodecContext::GetOnPacket(const Napi::CallbackInfo& info) {
    return onPacket.Value();
}

void NAVCodecContext::SetOnPacket(const Napi::CallbackInfo& info, const Napi::Value &value) {
    auto func = info[0].As<Napi::Function>();
    onPacket = Napi::Persistent(func);
    onPacketTSFN = Napi::ThreadSafeFunction::New(info.Env(), func, "AVContext#onPacket", 0, 1);
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

Napi::Value NAVCodecContext::GetBitRate(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->bit_rate);
}

Napi::Value NAVCodecContext::GetBitRateTolerance(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->bit_rate_tolerance);
}

Napi::Value NAVCodecContext::GetGlobalQuality(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->global_quality);
}

Napi::Value NAVCodecContext::GetCompressionLevel(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->compression_level);
}

Napi::Value NAVCodecContext::GetFlags(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->flags);
}

Napi::Value NAVCodecContext::GetFlags2(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->flags2);
}

Napi::Value NAVCodecContext::GetTimeBase(const Napi::CallbackInfo& info) {
    return NRational(info.Env(), GetHandle()->time_base);
}

Napi::Value NAVCodecContext::GetTicksPerFrame(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->ticks_per_frame);
}

Napi::Value NAVCodecContext::GetDelay(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->delay);
}

Napi::Value NAVCodecContext::GetWidth(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->width);
}

Napi::Value NAVCodecContext::GetHeight(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->height);
}

Napi::Value NAVCodecContext::GetCodedWidth(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->coded_width);
}

Napi::Value NAVCodecContext::GetCodedHeight(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->coded_height);
}

Napi::Value NAVCodecContext::GetGopSize(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->gop_size);
}

Napi::Value NAVCodecContext::GetPixelFormat(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->pix_fmt);
}

Napi::Value NAVCodecContext::GetMaxBFrames(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->max_b_frames);
}

Napi::Value NAVCodecContext::GetBQuantizationFactor(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->b_quant_factor);
}

Napi::Value NAVCodecContext::GetBQuantizationOffset(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->b_quant_offset);
}

Napi::Value NAVCodecContext::HasBFrames(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), GetHandle()->has_b_frames);
}

Napi::Value NAVCodecContext::GetIQuantizationFactor(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->i_quant_factor);
}

Napi::Value NAVCodecContext::GetIQuantizationOffset(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->i_quant_offset);
}

Napi::Value NAVCodecContext::GetLuminanceMasking(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->lumi_masking);
}

Napi::Value NAVCodecContext::GetTemporalComplexityMasking(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->temporal_cplx_masking);
}

Napi::Value NAVCodecContext::GetSpatialComplexityMasking(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->spatial_cplx_masking);
}

Napi::Value NAVCodecContext::GetPMasking(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->p_masking);
}

Napi::Value NAVCodecContext::GetDarkMasking(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->dark_masking);
}

Napi::Value NAVCodecContext::GetSampleAspectRatio(const Napi::CallbackInfo& info) {
    return NRational(info.Env(), GetHandle()->sample_aspect_ratio);
}

Napi::Value NAVCodecContext::GetMotionEstimationComparisonFunction(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->me_cmp);
}

Napi::Value NAVCodecContext::GetSubpixelMotionEstimationComparisonFunction(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->me_sub_cmp);
}

Napi::Value NAVCodecContext::GetMacroblockComparisonFunction(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->mb_cmp);
}

Napi::Value NAVCodecContext::GetInterlacedDCTComparisonFunction(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->ildct_cmp);
}

Napi::Value NAVCodecContext::GetDiamondSize(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->dia_size);
}

Napi::Value NAVCodecContext::GetLastPredictorCount(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->last_predictor_count);
}

Napi::Value NAVCodecContext::GetMotionEstimationPrepassComparisonFunction(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->me_pre_cmp);
}

Napi::Value NAVCodecContext::GetMotionEstimationPrepassDiamondSize(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->pre_dia_size);
}

Napi::Value NAVCodecContext::GetMotionEstimationSubpelQuality(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->me_subpel_quality);
}

Napi::Value NAVCodecContext::GetMotionEstimationRange(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->me_range);
}

Napi::Value NAVCodecContext::GetSliceFlags(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->slice_flags);
}

Napi::Value NAVCodecContext::GetMacroblockDecisionMode(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->mb_decision);
}

Napi::Value NAVCodecContext::GetIntraDCPrecision(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->intra_dc_precision);
}


Napi::Value NAVCodecContext::GetSkipTop(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->skip_top);
}

Napi::Value NAVCodecContext::GetSkipBottom(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->skip_bottom);
}

Napi::Value NAVCodecContext::GetMinMBLagrangeMultiplier(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->mb_lmin);
}

Napi::Value NAVCodecContext::GetMaxLBLagrangeMultiplier(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->mb_lmax);
}

Napi::Value NAVCodecContext::GetBidirectionalRefine(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->bidir_refine);
}

Napi::Value NAVCodecContext::GetMinGopSize(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->keyint_min);
}

Napi::Value NAVCodecContext::GetReferenceFrameCount(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->refs);
}

Napi::Value NAVCodecContext::GetMv0Threshold(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->mv0_threshold);
}

Napi::Value NAVCodecContext::GetColorPrimaries(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->color_primaries);
}

Napi::Value NAVCodecContext::GetColorTrc(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->color_trc);
}

Napi::Value NAVCodecContext::GetColorSpace(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->colorspace);
}

Napi::Value NAVCodecContext::GetColorRange(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->color_range);
}

Napi::Value NAVCodecContext::GetChromeSampleLocation(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->chroma_sample_location);
}

Napi::Value NAVCodecContext::GetSliceCount(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->slice_count);
}

Napi::Value NAVCodecContext::GetFieldOrder(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->field_order);
}

Napi::Value NAVCodecContext::GetSampleRate(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->sample_rate);
}

Napi::Value NAVCodecContext::GetSampleFormat(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->sample_fmt);
}

Napi::Value NAVCodecContext::GetFrameSize(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->frame_size);
}

Napi::Value NAVCodecContext::GetFrameNumber(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->frame_number);
}

Napi::Value NAVCodecContext::GetBlockAlignment(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->block_align);
}

Napi::Value NAVCodecContext::GetCutoff(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->cutoff);
}

Napi::Value NAVCodecContext::GetAudioServiceType(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->audio_service_type);
}

Napi::Value NAVCodecContext::GetRequestSampleFormat(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->request_sample_fmt);
}

Napi::Value NAVCodecContext::GetQuantizerCompression(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->qcompress);
}

Napi::Value NAVCodecContext::GetQuantizerBlur(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->qblur);
}

Napi::Value NAVCodecContext::GetMinQuantizer(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->qmin);
}

Napi::Value NAVCodecContext::GetMaxQuantizer(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->qmax);
}

Napi::Value NAVCodecContext::GetMaxQuantizerDifference(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->max_qdiff);
}

Napi::Value NAVCodecContext::GetRateControlBufferSize(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->rc_buffer_size);
}

Napi::Value NAVCodecContext::GetRateControlOverrideCount(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->rc_override_count);
}

Napi::Value NAVCodecContext::GetRateControlMaxBitrate(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->rc_max_rate);
}

Napi::Value NAVCodecContext::GetRateControlMinBitrate(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->rc_min_rate);
}

Napi::Value NAVCodecContext::GetRateControlMaxAvailableVbvUse(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->rc_max_available_vbv_use);
}

Napi::Value NAVCodecContext::GetRateControlMinVbvOverflowUse(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->rc_min_vbv_overflow_use);
}

Napi::Value NAVCodecContext::GetRateControlInitialBufferOccupancy(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->rc_initial_buffer_occupancy);
}

Napi::Value NAVCodecContext::GetTrellis(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->trellis);
}

Napi::Value NAVCodecContext::GetWorkaroundBugs(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->workaround_bugs);
}

Napi::Value NAVCodecContext::GetErrorRecognitionFlags(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->err_recognition);
}
