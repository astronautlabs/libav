#include "../libav.h"
#include "frame.h"
#include "buffer.h"

void NAVFrame::Init(Napi::Env env, Napi::Object exports) {
    auto func = DefineClass(env, "AVFrame", {
        InstanceAccessor("data", &NAVFrame::GetData, nullptr),
        InstanceAccessor("lineSize", &NAVFrame::GetLineSize, nullptr),
        InstanceAccessor("extendedData", &NAVFrame::GetExtendedData, nullptr),
        InstanceAccessor("width", &NAVFrame::GetWidth, nullptr),
        InstanceAccessor("height", &NAVFrame::GetHeight, nullptr),
        InstanceAccessor("numberOfSamples", &NAVFrame::GetNumberOfSamples, nullptr),
        InstanceAccessor("format", &NAVFrame::GetFormat, nullptr),
        InstanceAccessor("keyFrame", &NAVFrame::IsKeyFrame, nullptr),
        InstanceAccessor("pictureType", &NAVFrame::GetPictType, nullptr),
        InstanceAccessor("sampleAspectRatio", &NAVFrame::GetSampleAspectRatio, nullptr),
        InstanceAccessor("pts", &NAVFrame::GetPts, nullptr),
        InstanceAccessor("packetPts", &NAVFrame::GetPacketPts, nullptr),
        InstanceAccessor("timeBase", &NAVFrame::GetTimeBase, nullptr),
        InstanceAccessor("codedPictureNumber", &NAVFrame::GetCodedPictureNumber, nullptr),
        InstanceAccessor("displayPictureNumber", &NAVFrame::GetDisplayPictureNumber, nullptr),
        InstanceAccessor("quality", &NAVFrame::GetQuality, nullptr),
        InstanceAccessor("repeatPict", &NAVFrame::GetRepeatPict, nullptr),
        InstanceAccessor("interlaced", &NAVFrame::GetInterlacedFrame, nullptr),
        InstanceAccessor("topFieldFirst", &NAVFrame::GetTopFieldFirst, nullptr),
        InstanceAccessor("paletteHasChanged", &NAVFrame::HasPaletteChanged, nullptr),
        InstanceAccessor("reorderedOpaque", &NAVFrame::GetReorderedOpaque, nullptr),
        InstanceAccessor("sampleRate", &NAVFrame::GetSampleRate, nullptr),
        InstanceAccessor("buffers", &NAVFrame::GetBuffers, nullptr),
        InstanceAccessor("extendedBuffers", &NAVFrame::GetExtendedBuffers, nullptr),
        InstanceAccessor("sideData", &NAVFrame::GetSideData, nullptr),
        InstanceAccessor("flags", &NAVFrame::GetFlags, nullptr),
        InstanceAccessor("colorRange", &NAVFrame::GetColorRange, nullptr),
        InstanceAccessor("colorPrimaries", &NAVFrame::GetColorPrimaries, nullptr),
        InstanceAccessor("colorTransferCharacteristics", &NAVFrame::GetColorTrc, nullptr),
        InstanceAccessor("colorSpace", &NAVFrame::GetColorSpace, nullptr),
        InstanceAccessor("chromaLocation", &NAVFrame::GetChromaLocation, nullptr),
        InstanceAccessor("bestEffortTimestamp", &NAVFrame::GetBestEffortTimestamp, nullptr),
        InstanceAccessor("packetPosition", &NAVFrame::GetPacketPos, nullptr),
        InstanceAccessor("packetDuration", &NAVFrame::GetPacketDuration, nullptr),
        InstanceAccessor("metadata", &NAVFrame::GetMetadata, nullptr),
        InstanceAccessor("decodeErrorFlags", &NAVFrame::GetDecodeErrorFlags, nullptr),
        InstanceAccessor("packetSize", &NAVFrame::GetPacketSize, nullptr),
        InstanceAccessor("hwFramesContext", &NAVFrame::GetHwFramesContext, nullptr),
        InstanceAccessor("opaqueRef", &NAVFrame::GetOpaqueRef, nullptr),
        InstanceAccessor("cropTop", &NAVFrame::GetCropTop, nullptr),
        InstanceAccessor("cropBottom", &NAVFrame::GetCropBottom, nullptr),
        InstanceAccessor("cropLeft", &NAVFrame::GetCropLeft, nullptr),
        InstanceAccessor("cropRight", &NAVFrame::GetCropRight, nullptr),
        InstanceAccessor("channelLayout", &NAVFrame::GetChannelLayout, nullptr),
    });

    auto constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    
    LibAvAddon::Self(env)->AVFrame = constructor;
    exports.Set("AVFrame", constructor->Value());
}

NAVFrame::NAVFrame(const Napi::CallbackInfo& info):
    Napi::ObjectWrap<NAVFrame>(info)
{
    if (info.Length() > 0) {
        handle = info[0].As<Napi::External<AVFrame>>().Data();
    } else {
        handle = av_frame_alloc();
    }
}

void NAVFrame::Finalize(Napi::Env env) {
    av_frame_free(&handle);
}

Napi::Value NAVFrame::ReferTo(const Napi::CallbackInfo& info) {
    auto src = Unwrap(info[0].As<Napi::Object>());
    if (!src)
        return info.Env().Undefined();

    auto result = av_frame_ref(handle, src->handle);

    if (result < 0) {
        Napi::Error::New(info.Env(), "Failed to refer to frame: " + result).ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    // TODO

    return info.Env().Undefined();
}

Napi::Value NAVFrame::Unrefer(const Napi::CallbackInfo& info) {
    av_frame_unref(handle);
}

Napi::Value NAVFrame::Clone(const Napi::CallbackInfo& info) {
    auto newHandle = av_frame_clone(handle);

    if (!newHandle) {
        Napi::Error::New(info.Env(), "Failed to allocate cloned frame").ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    return LibAvAddon::Self(info.Env())->AVFrame->New({ 
        Napi::External<AVFrame>::New(info.Env(), newHandle)
    });
}

Napi::Value NAVFrame::CopyTo(const Napi::CallbackInfo& info) {
    auto other = NAVFrame::Unwrap(info[0].As<Napi::Object>());
    uint32_t result = av_frame_copy(other->handle, handle);

    if (result < 0) {
        Napi::Error::New(info.Env(), "Failed to copy to frame: " + result).ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }
}

Napi::Value NAVFrame::CopyPropertiesTo(const Napi::CallbackInfo& info) {
    auto other = NAVFrame::Unwrap(info[0].As<Napi::Object>());
    uint32_t result = av_frame_copy_props(other->handle, handle);

    if (result < 0) {
        Napi::Error::New(info.Env(), "Failed to copy to frame: " + result).ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }
}

Napi::Value NAVFrame::GetPlaneBuffer(const Napi::CallbackInfo& info) {
    auto plane = info[0].ToNumber();
    auto buffer = av_frame_get_plane_buffer(handle, plane);
    if (!buffer) {
        Napi::Error::New(info.Env(), "Failed to get plane buffer: Frame is not valid")
            .ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    return NAVBuffer::FromHandle(info.Env(), buffer, true);
}

Napi::Value NAVFrame::AddSideData(const Napi::CallbackInfo& info) {
    auto type = (enum AVFrameSideDataType)info[0].ToNumber().Int32Value();
    auto size = (enum AVFrameSideDataType)info[1].ToNumber().Uint32Value();
    auto sideData = av_frame_new_side_data(handle, type, size);

    if (!sideData) {
        Napi::Error::New(info.Env(), "Failed to add side data")
            .ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    return info.Env().Undefined(); // TODO
    //return NAVFrameSideData::FromHandle(info.Env(), sideData);
}

Napi::Value NAVFrame::GetSideData(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::RemoveSideData(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::ApplyCropping(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetSideDataName(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}


Napi::Value NAVFrame::MoveReference(const Napi::CallbackInfo& info) {
    auto src = Unwrap(info[0].As<Napi::Object>());
    if (!src)
        return info.Env().Undefined();

    av_frame_move_ref(handle, src->handle);
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetBuffer(const Napi::CallbackInfo& info) {
    auto alignment = 0;
    if (info.Length() > 0)
        alignment = info[0].As<Napi::Number>().Int32Value();
    
    auto result = av_frame_get_buffer(handle, alignment);

    if (result < 0) {
        Napi::Error::New(info.Env(), "Failed to allocate cloned frame").ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    // TODO wait what
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetData(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetLineSize(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetExtendedData(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetWidth(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetHeight(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetNumberOfSamples(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetFormat(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::IsKeyFrame(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetPictType(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetSampleAspectRatio(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetPts(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetPacketPts(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetTimeBase(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetCodedPictureNumber(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetDisplayPictureNumber(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetQuality(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetRepeatPict(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetInterlacedFrame(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}


Napi::Value NAVFrame::IsInterlacedFrame(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::IsTopFieldFirst(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetTopFieldFirst(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::HasPaletteChanged(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetReorderedOpaque(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetSampleRate(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetBuffers(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetExtendedBuffers(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetSideDatas(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetFlags(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetColorRange(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetColorPrimaries(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetColorTrc(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetColorSpace(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetChromaLocation(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetBestEffortTimestamp(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetPacketPos(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetPacketDuration(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetMetadata(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetDecodeErrorFlags(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetPacketSize(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetHwFramesContext(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetOpaqueRef(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetCropTop(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetCropBottom(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetCropLeft(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetCropRight(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVFrame::GetChannelLayout(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

// Napi::Value NAVFrameSideData::FromHandle(const Napi::Env &env, AVFrameSideData *handle) {
//     auto addon = LibAvAddon::Self(env);
//     auto resource = addon->GetResource<NAVFrameSideData>(handle);
//     if (resource)
//         return resource->Value();
    
//     if (!resource) {
//         return addon->AVFrameSideData->New({ Napi::External<AVFrameSideData>::New(env, handle) });
//     }

// }

// void NAVFrameSideData::Init(Napi::Env env, Napi::Object exports) {
//     auto func = DefineClass(env, "AVFrameSideData", {
//         InstanceAccessor("type", &NAVFrameSideData::GetType, nullptr),
//         InstanceAccessor("data", &NAVFrameSideData::GetData, nullptr),
//         InstanceAccessor("size", &NAVFrameSideData::GetSize, nullptr),
//         InstanceAccessor("metadata", &NAVFrameSideData::GetMetadata, nullptr),
//         InstanceAccessor("buffer", &NAVFrameSideData::GetBuffer, nullptr),
//     });

//     auto constructor = new Napi::FunctionReference();
//     *constructor = Napi::Persistent(func);
    
//     LibAvAddon::Self(env)->AVFrameSideData = constructor;
//     exports.Set("AVFrameSideData", constructor->Value());
// }