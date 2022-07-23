#include "../libav.h"
#include "frame.h"
#include "buffer.h"
#include "../avutil/dict.h"
#include "../avutil/channel-layout.h"

#include <assert.h>

NAVFrame::NAVFrame(const Napi::CallbackInfo& info):
    NAVResource(info)
{
    if (ConstructFromHandle(info))
        return;

    SetHandle(av_frame_alloc());
}

void NAVFrame::Free() {
    auto handle = GetHandle();
    av_frame_free(&handle);
    SetHandle(handle);
}

void NAVFrame::RefHandle() {
    auto newFrame = av_frame_alloc();
    assert(0 == av_frame_ref(newFrame, GetHandle()));
    SetHandle(newFrame);
    Owned = false;
}

Napi::Value NAVFrame::ReferTo(const Napi::CallbackInfo& info) {
    auto src = Unwrap(info[0].As<Napi::Object>());
    if (!src)
        return info.Env().Undefined();

    auto handle = GetHandle();
    auto srcHandle = src->GetHandle();
    auto result = av_frame_ref(handle, srcHandle);

    if (result < 0)
        return nlav_throw(info.Env(), result, "av_frame_ref");

    return info.Env().Undefined();
}

Napi::Value NAVFrame::Unrefer(const Napi::CallbackInfo& info) {
    av_frame_unref(GetHandle());
    return info.Env().Undefined();
}

Napi::Value NAVFrame::Clone(const Napi::CallbackInfo& info) {
    return NAVFrame::FromHandleWrapped(info.Env(), av_frame_clone(GetHandle()), true);
}

Napi::Value NAVFrame::CopyTo(const Napi::CallbackInfo& info) {
    auto other = NAVFrame::Unwrap(info[0].As<Napi::Object>());
    auto otherHandle = other->GetHandle();
    uint32_t result = av_frame_copy(otherHandle, GetHandle());

    if (result < 0)
        nlav_throw(info.Env(), result, "av_frame_copy");

    return info.Env().Undefined();
}

Napi::Value NAVFrame::CopyPropertiesTo(const Napi::CallbackInfo& info) {
    auto other = NAVFrame::Unwrap(info[0].As<Napi::Object>());
    uint32_t result = av_frame_copy_props(other->GetHandle(), GetHandle());

    if (result < 0)
        return nlav_throw(info.Env(), result, "av_frame_copy_props");

    return info.Env().Undefined();
}

Napi::Value NAVFrame::GetPlaneBuffer(const Napi::CallbackInfo& info) {
    auto plane = info[0].ToNumber();
    auto buffer = av_frame_get_plane_buffer(GetHandle(), plane);
    if (!buffer) {
        Napi::Error::New(info.Env(), "Failed to get plane buffer: Frame is not valid")
            .ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    return NAVBuffer::FromHandleWrapped(info.Env(), buffer, true);
}

Napi::Value NAVFrame::AddSideData(const Napi::CallbackInfo& info) {
    auto type = (enum AVFrameSideDataType)info[0].ToNumber().Int32Value();

    AVFrameSideData *sideData;

    if (info[1].IsNumber()) {
        auto size = (enum AVFrameSideDataType)info[1].ToNumber().Uint32Value();
        sideData = av_frame_new_side_data(GetHandle(), type, size);
    } else {
        auto buf = NAVBuffer::Unwrap(info[1].As<Napi::Object>());
        sideData = av_frame_new_side_data_from_buf(GetHandle(), type, buf->GetHandle());
    }

    if (!sideData) {
        Napi::Error::New(info.Env(), "Failed to add side data")
            .ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    return NAVFrameSideData::FromHandleWrapped(info.Env(), sideData, false);
}

Napi::Value NAVFrame::GetSideData(const Napi::CallbackInfo& info) {
    auto type = (AVFrameSideDataType)info[0].As<Napi::Number>().Int32Value();
    return NAVFrameSideData::FromHandleWrapped(info.Env(), av_frame_get_side_data(GetHandle(), type), false);
}

Napi::Value NAVFrame::RemoveSideData(const Napi::CallbackInfo& info) {
    auto type = (AVFrameSideDataType)info[0].As<Napi::Number>().Int32Value();
    av_frame_remove_side_data(GetHandle(), type);
    return info.Env().Undefined();
}

Napi::Value NAVFrame::ApplyCropping(const Napi::CallbackInfo& info) {
    int flags = 0;
    if (info.Length() > 0)
        flags = info[0].As<Napi::Number>().Int32Value();
    
    int result = av_frame_apply_cropping(GetHandle(), flags);
    if (result < 0)
        nlav_throw(info.Env(), result, "av_frame_apply_cropping");
    
    return info.Env().Undefined();
}

Napi::Value NAVFrame::GetSideDataName(const Napi::CallbackInfo& info) {
    auto type = (AVFrameSideDataType)info[0].As<Napi::Number>().Int32Value();
    return Napi::String::New(info.Env(), av_frame_side_data_name(type));
}

Napi::Value NAVFrame::MoveReferenceFrom(const Napi::CallbackInfo& info) {
    auto src = Unwrap(info[0].As<Napi::Object>());
    if (!src)
        return info.Env().Undefined();

    // TODO: are all the memory implications OK here?
    av_frame_move_ref(GetHandle(), src->GetHandle());
    return info.Env().Undefined();
}

Napi::Value NAVFrame::AllocateBuffer(const Napi::CallbackInfo& info) {
    auto alignment = 0;
    if (info.Length() > 0)
        alignment = info[0].As<Napi::Number>().Int32Value();
    
    auto result = av_frame_get_buffer(GetHandle(), alignment);
    if (result < 0)
        return nlav_throw(info.Env(), result, "av_frame_get_buffer");

    return info.Env().Undefined();
}

Napi::Value NAVFrame::IsWritable(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), av_frame_is_writable(GetHandle()));
}

// Properties //////////////////////////////////////////////////////////////////////////

Napi::Value NAVFrame::GetLineSize(const Napi::CallbackInfo& info) {
    std::vector<int> vec;
    
    for (int i = 0, max = AV_NUM_DATA_POINTERS; i < max; ++i)
        vec.push_back(GetHandle()->linesize[i]);
    
    return VectorToArray(
        info.Env(),
        Transform<int, Napi::Value>(
            vec,
            WrapNumbers<int>(info.Env())
        )
    );
}

void NAVFrame::SetLineSize(const Napi::CallbackInfo& info, const Napi::Value& value) {
    auto array = value.As<Napi::Array>();

    if (array.Length() >= AV_NUM_DATA_POINTERS) {
        std::string message = "Too many line sizes provided, max is " + (AV_NUM_DATA_POINTERS);
        Napi::TypeError::New(info.Env(), message);
        return;
    }

    auto handle = GetHandle();
    for (uint32_t i = 0, max = AV_NUM_DATA_POINTERS; i < max; ++i) {
        if (array.Length() <= i) {
            handle->linesize[i] = 0;
        } else {
            handle->linesize[i] = array.Get(i).As<Napi::Number>().Int32Value();
        }
    }
}

Napi::Value NAVFrame::GetWidth(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->width);
}

void NAVFrame::SetWidth(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->width = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVFrame::GetHeight(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->height);
}

void NAVFrame::SetHeight(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->height = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVFrame::GetNumberOfSamples(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->nb_samples);
}

void NAVFrame::SetNumberOfSamples(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->nb_samples = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVFrame::GetFormat(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->format);
}

void NAVFrame::SetFormat(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->format = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVFrame::GetIsKeyFrame(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), GetHandle()->key_frame);
}

void NAVFrame::SetIsKeyFrame(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->key_frame = value.ToBoolean().Value();
}

Napi::Value NAVFrame::GetPictType(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->pict_type);
}

void NAVFrame::SetPictType(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->pict_type = (AVPictureType)value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVFrame::GetSampleAspectRatio(const Napi::CallbackInfo& info) {
    return NRational(info.Env(), GetHandle()->sample_aspect_ratio);
}

void NAVFrame::SetSampleAspectRatio(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->sample_aspect_ratio = FromNRational(info.Env(), value);
}

Napi::Value NAVFrame::GetPts(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->pts);
}

void NAVFrame::SetPts(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->pts = value.As<Napi::Number>().Int64Value();
}

Napi::Value NAVFrame::GetPacketDts(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->pkt_dts);
}

void NAVFrame::SetPacketDts(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->pkt_dts = value.As<Napi::Number>().Int64Value();
}

Napi::Value NAVFrame::GetTimeBase(const Napi::CallbackInfo& info) {
    return NRational(info.Env(), GetHandle()->time_base);
}

void NAVFrame::SetTimeBase(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->time_base = FromNRational(info.Env(), value);
}

Napi::Value NAVFrame::GetCodedPictureNumber(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->coded_picture_number);
}

void NAVFrame::SetCodedPictureNumber(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->coded_picture_number = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVFrame::GetDisplayPictureNumber(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->display_picture_number);
}

void NAVFrame::SetDisplayPictureNumber(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->display_picture_number = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVFrame::GetQuality(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->quality);
}

void NAVFrame::SetQuality(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->quality = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVFrame::GetRepeatPict(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->repeat_pict);
}

void NAVFrame::SetRepeatPict(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->repeat_pict = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVFrame::GetInterlacedFrame(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), GetHandle()->interlaced_frame);
}

void NAVFrame::SetInterlacedFrame(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->interlaced_frame = value.ToBoolean().Value();
}

Napi::Value NAVFrame::GetTopFieldFirst(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), GetHandle()->top_field_first);
}

void NAVFrame::SetTopFieldFirst(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->top_field_first = value.ToBoolean().Value();
}

Napi::Value NAVFrame::GetHasPaletteChanged(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), GetHandle()->palette_has_changed);
}

void NAVFrame::SetHasPaletteChanged(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->palette_has_changed = value.ToBoolean().Value();
}

Napi::Value NAVFrame::GetReorderedOpaque(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->reordered_opaque);
}

void NAVFrame::SetReorderedOpaque(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->reordered_opaque = value.As<Napi::Number>().Int64Value();
}

Napi::Value NAVFrame::GetSampleRate(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->sample_rate);
}

void NAVFrame::SetSampleRate(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->sample_rate = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVFrame::GetBuffers(const Napi::CallbackInfo& info) {
    std::vector<AVBufferRef*> bufs;
    auto handle = GetHandle();

    for (int i = 0, max = AV_NUM_DATA_POINTERS; i < max; ++i) {
        if (!handle->buf[i])
            break;
        bufs.push_back(handle->buf[i]);
    }

    return VectorToArray(
        info.Env(),
        Transform<AVBufferRef*, Napi::Value>(bufs, [&](AVBufferRef *buf) {
            return NAVBuffer::FromHandleWrapped(info.Env(), buf, false);
        })
    );
}

Napi::Value NAVFrame::GetExtendedBuffers(const Napi::CallbackInfo& info) {
    std::vector<AVBufferRef*> bufs;
    auto handle = GetHandle();

    for (int i = 0, max = handle->nb_extended_buf; i < max; ++i) {
        if (!handle->extended_buf[i])
            break;
        bufs.push_back(handle->extended_buf[i]);
    }

    return VectorToArray(
        info.Env(),
        Transform<AVBufferRef*, Napi::Value>(bufs, [&](AVBufferRef *buf) {
            return NAVBuffer::FromHandleWrapped(info.Env(), buf, false);
        })
    );
}

Napi::Value NAVFrame::GetSideDatas(const Napi::CallbackInfo& info) {
    std::vector<Napi::Value> vec;
    auto handle = GetHandle();

    for (int i = 0, max = GetHandle()->nb_side_data; i < max; ++i) {
        if (handle->side_data[i])
            vec.push_back(NAVFrameSideData::FromHandleWrapped(info.Env(), handle->side_data[i], false));
    }

    return VectorToArray(info.Env(), vec);
}

Napi::Value NAVFrame::GetFlags(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->flags);
}

void NAVFrame::SetFlags(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->flags = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVFrame::GetColorRange(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->color_range);
}

void NAVFrame::SetColorRange(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->color_range = (AVColorRange)value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVFrame::GetColorPrimaries(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->color_primaries);
}

void NAVFrame::SetColorPrimaries(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->color_primaries = (AVColorPrimaries)value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVFrame::GetColorTrc(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->color_trc);
}

void NAVFrame::SetColorTrc(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->color_trc = (AVColorTransferCharacteristic)value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVFrame::GetColorSpace(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->colorspace);
}

void NAVFrame::SetColorSpace(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->colorspace = (AVColorSpace)value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVFrame::GetChromaLocation(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->chroma_location);
}

void NAVFrame::SetChromaLocation(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->chroma_location = (AVChromaLocation)value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVFrame::GetBestEffortTimestamp(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->best_effort_timestamp);
}

void NAVFrame::SetBestEffortTimestamp(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->best_effort_timestamp = value.As<Napi::Number>().Int64Value();
}

Napi::Value NAVFrame::GetPacketPos(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->pkt_pos);
}

void NAVFrame::SetPacketPos(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->pkt_pos = value.As<Napi::Number>().Int64Value();
}

Napi::Value NAVFrame::GetPacketDuration(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->pkt_duration);
}

void NAVFrame::SetPacketDuration(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->pkt_duration = value.As<Napi::Number>().Int64Value();
}

Napi::Value NAVFrame::GetMetadata(const Napi::CallbackInfo& info) {
    return NAVDictionary::FromHandleWrapped(info.Env(), GetHandle()->metadata, false);
}

void NAVFrame::SetMetadata(const Napi::CallbackInfo& info, const Napi::Value& value) {
    auto dict = NAVDictionary::Unwrap(value.As<Napi::Object>());
    GetHandle()->metadata = dict->GetHandle();
}

Napi::Value NAVFrame::GetDecodeErrorFlags(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->decode_error_flags);
}

void NAVFrame::SetDecodeErrorFlags(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->decode_error_flags = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVFrame::GetPacketSize(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->pkt_size);
}

void NAVFrame::SetPacketSize(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->pkt_size = value.As<Napi::Number>().Int32Value();
}

Napi::Value NAVFrame::GetHwFramesContext(const Napi::CallbackInfo& info) {
    return NAVBuffer::FromHandleWrapped(info.Env(), GetHandle()->hw_frames_ctx, false);
}

void NAVFrame::SetHwFramesContext(const Napi::CallbackInfo& info, const Napi::Value& value) {
    auto buf = NAVBuffer::Unwrap(value.As<Napi::Object>());
    GetHandle()->hw_frames_ctx = buf->GetHandle();
}

Napi::Value NAVFrame::GetOpaqueRef(const Napi::CallbackInfo& info) {
    return NAVBuffer::FromHandleWrapped(info.Env(), GetHandle()->opaque_ref, false);
}

void NAVFrame::SetOpaqueRef(const Napi::CallbackInfo& info, const Napi::Value& value) {
    auto buf = NAVBuffer::Unwrap(value.As<Napi::Object>());
    GetHandle()->opaque_ref = buf->GetHandle();
}

Napi::Value NAVFrame::GetCropTop(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->crop_top);
}

void NAVFrame::SetCropTop(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->crop_top = value.As<Napi::Number>().Int64Value();
}

Napi::Value NAVFrame::GetCropBottom(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->crop_bottom);
}

void NAVFrame::SetCropBottom(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->crop_bottom = value.As<Napi::Number>().Int64Value();
}

Napi::Value NAVFrame::GetCropLeft(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->crop_left);
}

void NAVFrame::SetCropLeft(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->crop_left = value.As<Napi::Number>().Int64Value();
}

Napi::Value NAVFrame::GetCropRight(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->crop_right);
}

void NAVFrame::SetCropRight(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->crop_right = value.As<Napi::Number>().Int64Value();
}

Napi::Value NAVFrame::GetChannelLayout(const Napi::CallbackInfo& info) {
#ifdef FFMPEG_5_1
    return NAVChannelLayout::FromHandleWrapped(info.Env(), &GetHandle()->ch_layout, false);
#else
    return Napi::Number::New(info.Env(), GetHandle()->channel_layout);
#endif
}

void NAVFrame::SetChannelLayout(const Napi::CallbackInfo& info, const Napi::Value& value) {
#ifdef FFMPEG_5_1
    auto layout = NAVChannelLayout::Unwrap(value.As<Napi::Object>());
    GetHandle()->ch_layout = *layout->GetHandle();
#else
    auto layout = info[0].As<Napi::Number>().Int64Value();
    GetHandle()->channel_layout = (uint64_t)layout;
#endif
}

NAVFrameSideData::NAVFrameSideData(const Napi::CallbackInfo &info):
    NAVResource(info)
{
    if (ConstructFromHandle(info))
        return;
}

void NAVFrameSideData::Free() {
    // Do nothing, we never own these.
}

Napi::Value NAVFrameSideData::GetType(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->type);
}

Napi::Value NAVFrameSideData::GetSize(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->size);
}

Napi::Value NAVFrameSideData::GetMetadata(const Napi::CallbackInfo& info) {
    return NAVDictionary::FromHandleWrapped(info.Env(), GetHandle()->metadata, false);
}

Napi::Value NAVFrameSideData::GetBuffer(const Napi::CallbackInfo& info) {
    return NAVBuffer::FromHandleWrapped(info.Env(), GetHandle()->buf, false);
}
