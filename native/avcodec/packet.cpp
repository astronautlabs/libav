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

Napi::Value NAVPacket::GetBuffer(const Napi::CallbackInfo& info) {
    return NAVBuffer::FromHandleWrapped(info.Env(), GetHandle()->buf, false);
}

Napi::Value NAVPacket::GetPts(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->pts);
}

Napi::Value NAVPacket::GetDts(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->dts);
}

Napi::Value NAVPacket::GetSize(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->size);
}

Napi::Value NAVPacket::GetStreamIndex(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->stream_index);
}

Napi::Value NAVPacket::GetFlags(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->flags);
}

Napi::Value NAVPacket::GetSideData(const Napi::CallbackInfo& info) {
    return NAVPacketSideData::FromHandlesWrapped(info.Env(), GetHandle()->side_data, GetHandle()->side_data_elems, false);
}

Napi::Value NAVPacket::GetSideDataCount(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->side_data_elems);
}

Napi::Value NAVPacket::Duration(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->duration);
}

Napi::Value NAVPacket::GetPosition(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->pos);
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
