#include "packet.h"
#include "../avutil/buffer.h"

NAVPacket::NAVPacket(const Napi::CallbackInfo& info):
    NAVResource(info)
{
    if (ConstructFromHandle(info)) {
        return;
    }
    
    SetHandle(av_packet_alloc());

    if (info.Length() > 0) {
        auto externalBuffer = info[0].As<Napi::Uint8Array>();
        auto length = externalBuffer.ByteLength();

        uint8_t *buffer = (uint8_t*)av_malloc(length);
        memcpy(buffer, externalBuffer.Data(), length);
        av_packet_from_data(GetHandle(), buffer, length);
    }
}

void NAVPacket::Free() {
    auto handle = GetHandle();
    av_packet_free(&handle);
    SetHandle(handle);
}

void NAVPacket::RefHandle() {
    // Some AVPackets do not have a buf reference (only data+size).
    // - Since we need NAVBuffer to safely (and singly) expose a data buffer into
    //   Javascript land, we must ensure that there is an AVBufferRef allocated on `buf`.
    // - We also rely on ref-counting to ensure that the lifetime of the buffer will match
    //   the lifetime of the NAVPacket instance.
    av_packet_make_refcounted(GetHandle());
    SetHandle(av_packet_clone(GetHandle()));
}

// Methods

Napi::Value NAVPacket::AddSideData(const Napi::CallbackInfo& info) {
    auto type = (AVPacketSideDataType)info[0].As<Napi::Number>().Int32Value();
    auto buffer = info[1].As<Napi::ArrayBuffer>();
    auto size = buffer.ByteLength();
    auto data = (uint8_t*)av_malloc(size);
    memcpy(data, buffer.Data(), size);

    av_packet_add_side_data(GetHandle(), type, data, size);
    
    AVPacketSideData *sideData;

    // NOTE: Find the _last_ matching side data to ensure we return what was just pushed in.
    
    for (int i = 0; i < GetHandle()->side_data_elems; i++) {
        if (GetHandle()->side_data[i].type != type)
            continue;
        
        sideData = GetHandle()->side_data + i;
    }

    if (!sideData)
        return info.Env().Null();

    return NAVPacketSideData::FromHandleWrapped(info.Env(), sideData, false);
}

Napi::Value NAVPacket::GetSideData(const Napi::CallbackInfo& info) {
    auto type = (AVPacketSideDataType)info[0].As<Napi::Number>().Int32Value();
    AVPacketSideData* sideData = nullptr;

    for (int i = 0; i < GetHandle()->side_data_elems; i++) {
        if (GetHandle()->side_data[i].type == type) {
            sideData = GetHandle()->side_data + i;
            break;
        }
    }

    if (!sideData)
        return info.Env().Null();

    return NAVPacketSideData::FromHandleWrapped(info.Env(), sideData, false);
}

// Properties

Napi::Value NAVPacket::GetBuffer(const Napi::CallbackInfo& info) {
    return NAVBuffer::FromHandleWrapped(info.Env(), GetHandle()->buf, false);
}

Napi::Value NAVPacket::GetPts(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->pts);
}

void NAVPacket::SetPts(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->pts = info[0].As<Napi::Number>().Int64Value();
}

Napi::Value NAVPacket::GetDts(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->dts);
}

void NAVPacket::SetDts(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->dts = info[0].As<Napi::Number>().Int64Value();
}

Napi::Value NAVPacket::GetSize(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->size);
}

Napi::Value NAVPacket::GetStreamIndex(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->stream_index);
}

void NAVPacket::SetStreamIndex(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->stream_index = info[0].As<Napi::Number>().Int32Value();
}

Napi::Value NAVPacket::GetFlags(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->flags);
}

void NAVPacket::SetFlags(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->flags = info[0].As<Napi::Number>().Int32Value();
}

Napi::Value NAVPacket::GetSideDatas(const Napi::CallbackInfo& info) {
    return NAVPacketSideData::FromHandlesWrapped(info.Env(), GetHandle()->side_data, GetHandle()->side_data_elems, false);
}

Napi::Value NAVPacket::GetDuration(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->duration);
}

void NAVPacket::SetDuration(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->duration = info[0].As<Napi::Number>().Int64Value();
}

Napi::Value NAVPacket::GetPosition(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->pos);
}

void NAVPacket::SetPosition(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->pos = info[0].As<Napi::Number>().Int64Value();
}

Napi::Value NAVPacket::GetOpaqueBuffer(const Napi::CallbackInfo& info) {
    return NAVBuffer::FromHandleWrapped(info.Env(), GetHandle()->opaque_ref, false);
}

void NAVPacket::SetOpaqueBuffer(const Napi::CallbackInfo& info, const Napi::Value &value) {
    GetHandle()->opaque_ref = av_buffer_ref(NAVBuffer::Unwrap(info[0].As<Napi::Object>())->GetHandle());
}

Napi::Value NAVPacket::GetTimeBase(const Napi::CallbackInfo& info) {
    return NRational(info.Env(), GetHandle()->time_base);
}

void NAVPacket::SetTimeBase(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->time_base = FromNRational(info.Env(), info[0]);
}

NAVPacketSideData::NAVPacketSideData(const Napi::CallbackInfo& info):
    NAVResource(info)
{
    if (ConstructFromHandle(info)) {
        if (!ownedArrayBuffer.IsEmpty())
            ownedArrayBuffer.Unref();

        auto handle = GetHandle();
        ownedArrayBuffer = Napi::Reference<Napi::ArrayBuffer>::New(
            Napi::ArrayBuffer::New(info.Env(), handle->data, handle->size),
            1
        );
        return;
    }
}

void NAVPacketSideData::Free() {
    ownedArrayBuffer.Unref();
}

Napi::Value NAVPacketSideData::GetData(const Napi::CallbackInfo& info) {
    return ownedArrayBuffer.Value();
}

Napi::Value NAVPacketSideData::GetSize(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->size);
}

Napi::Value NAVPacketSideData::GetType(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->type);
}
