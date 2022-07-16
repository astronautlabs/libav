#include "../common.h"

extern "C" {
    #include <libavutil/avutil.h>
}

#include "buffer.h"
#include "../libav.h"
#include "../helpers.h"

void *GetRegisterableBufferHandle(void *bufferRef) {
    return (void*)((AVBufferRef*)bufferRef)->buffer;
}

NAVBuffer::NAVBuffer(const Napi::CallbackInfo &callback):
    NAVResource(callback)
{
    if (ConstructFromHandle(callback)) {
        if (!ownedArrayBuffer.IsEmpty())
            ownedArrayBuffer.Unref();

        auto handle = GetHandle();
        ownedArrayBuffer = Napi::Reference<Napi::ArrayBuffer>::New(
            Napi::ArrayBuffer::New(callback.Env(), handle->data, handle->size),
            1
        );

        return;
    }
    
    if (callback[0].IsNumber()) {
        auto size = callback[0].As<Napi::Number>().Int64Value();
        auto zero = false;

        if (callback.Length() > 1)
            zero = callback[1].As<Napi::Boolean>().Value();

        if (size < 0) {
            Napi::TypeError::New(callback.Env(), "Size cannot be negative").ThrowAsJavaScriptException();
            return;
        }

        AVBufferRef *buf_ref;

        if (zero)
            buf_ref = av_buffer_allocz(size);
        else
            buf_ref = av_buffer_alloc(size);

        if (!buf_ref) {
            Napi::TypeError::New(callback.Env(), "Failed to allocate buffer").ThrowAsJavaScriptException();
            return;
        }

        SetHandle(buf_ref);

        if (!ownedArrayBuffer.IsEmpty())
            ownedArrayBuffer.Unref();
        ownedArrayBuffer = Napi::Reference<Napi::ArrayBuffer>::New(
            Napi::ArrayBuffer::New(callback.Env(), buf_ref->data, buf_ref->size),
            1
        );
    } else if (callback[0].IsTypedArray() || callback[0].IsArrayBuffer()) {
        Napi::ArrayBuffer buffer;

        if (callback[0].IsTypedArray())
            buffer = callback[0].As<Napi::TypedArray>().ArrayBuffer();
        else if (callback[0].IsBuffer())
            buffer = callback[0].As<Napi::Buffer<uint8_t>>().ArrayBuffer();
        else
            buffer = callback[0].As<Napi::ArrayBuffer>();

        if (!ownedArrayBuffer.IsEmpty())
            ownedArrayBuffer.Unref();
        ownedArrayBuffer = Napi::Reference<Napi::ArrayBuffer>::New(buffer, 1);
        SetHandle(
            av_buffer_create(
                (uint8_t*)buffer.Data(), 
                buffer.ByteLength(), 
                &NAVBuffer::Disown, 
                this, 
                0
            )
        );
    } else {
        Napi::TypeError::New(callback.Env(), "Invalid invocation").ThrowAsJavaScriptException();
        return;
    }
}

void NAVBuffer::Free() {
    auto handle = GetHandle();
    av_buffer_unref(&handle);
    SetHandle(handle);
}

void NAVBuffer::RefHandle() {
    SetHandle(av_buffer_ref(GetHandle()));
}

void NAVBuffer::Disown(void *opaque, uint8_t *data) {
    NAVBuffer *buffer = (NAVBuffer*)opaque;
    if (!buffer->ownedArrayBuffer.IsEmpty())
        buffer->ownedArrayBuffer.Unref();
    buffer->ownedArrayBuffer = Napi::Reference<Napi::ArrayBuffer>();
    buffer->SetHandle(nullptr);
}

Napi::Value NAVBuffer::GetRefCount(const Napi::CallbackInfo& info) {
    if (!this->GetHandle())
        return Napi::Number::New(info.Env(), 0);
    
    return Napi::Number::New(info.Env(), av_buffer_get_ref_count(this->GetHandle()));
}

Napi::Value NAVBuffer::GetIsWritable(const Napi::CallbackInfo& info) {
    if (!this->GetHandle())
        return Napi::Boolean::New(info.Env(), false);
    
    return Napi::Boolean::New(info.Env(), av_buffer_is_writable(this->GetHandle()) == 1);
}

Napi::Value NAVBuffer::MakeWritable(const Napi::CallbackInfo& info) {
    auto handle = GetHandle();

    if (!handle) {
        Napi::TypeError::New(info.Env(), "Cannot make buffer writable after it has been freed").ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    if (av_buffer_is_writable(handle))
        return info.Env().Undefined();
    
    if (av_buffer_make_writable(&handle) < 0) {
        Napi::TypeError::New(info.Env(), "Failure while making buffer writable").ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    SetHandle(handle);

    if (!ownedArrayBuffer.IsEmpty())
        ownedArrayBuffer.Unref();
    ownedArrayBuffer = Napi::Reference<Napi::ArrayBuffer>();
    return info.Env().Undefined();
}

Napi::Value NAVBuffer::Realloc(const Napi::CallbackInfo& info) {
    auto size = info[0].As<Napi::Number>().Int64Value();
    auto handle = GetHandle();

    if (size < 0) {
        Napi::TypeError::New(info.Env(), "Size cannot be negative").ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }
    
    if (av_buffer_realloc(&handle, size) < 0) {
        Napi::TypeError::New(info.Env(), "Error while reallocating").ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    SetHandle(handle);

    if (!ownedArrayBuffer.IsEmpty())
        ownedArrayBuffer.Unref();
    ownedArrayBuffer = Napi::Reference<Napi::ArrayBuffer>::New(Napi::ArrayBuffer::New(info.Env(), handle->data, handle->size), 1);
    return info.Env().Undefined();
}

Napi::Value NAVBuffer::Replace(const Napi::CallbackInfo& info) {
    auto other = Napi::ObjectWrap<NAVBuffer>::Unwrap(info[0].As<Napi::Object>());
    auto handle = GetHandle();
    auto otherHandle = other->GetHandle();

    if (this == other || handle->buffer == otherHandle->buffer)
        return info.Env().Undefined();
    
    uint32_t result = av_buffer_replace(&handle, otherHandle);
    if (result < 0) {
        Napi::TypeError::New(info.Env(), "Error while replacing: " + result).ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    SetHandle(handle);

    if (!ownedArrayBuffer.IsEmpty())
        ownedArrayBuffer.Unref();
    ownedArrayBuffer = Napi::Reference<Napi::ArrayBuffer>();
    return info.Env().Undefined();
}


Napi::Value NAVBuffer::GetSize(const Napi::CallbackInfo& info) {
    auto handle = GetHandle();

    if (!handle) {
        Napi::TypeError::New(info.Env(), "Cannot access size after buffer has been freed.");
        return info.Env().Undefined();
    }

    return Napi::Number::New(info.Env(), handle->size);
}

Napi::Value NAVBuffer::GetData(const Napi::CallbackInfo& info) {
    auto handle = GetHandle();

    if (!handle) {
        Napi::TypeError::New(info.Env(), "Cannot access data after buffer has been freed.").ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }
    
    return ownedArrayBuffer.Value();
}

Napi::Value NAVBuffer::Free(const Napi::CallbackInfo& info) {
    auto handle = GetHandle();

    if (handle)
        av_buffer_unref(&handle);
    
    SetHandle(handle);

    if (!ownedArrayBuffer.IsEmpty())
        this->ownedArrayBuffer.Unref();
    this->ownedArrayBuffer = Napi::Reference<Napi::ArrayBuffer>();
    return info.Env().Undefined();
}

NAVBufferPool::NAVBufferPool(const Napi::CallbackInfo& info):
    NAVResource(info)
{
    if (ConstructFromHandle(info))
        return;
    
    auto size = info[0].As<Napi::Number>().Int64Value();
    
    if (size < 0) {
        Napi::TypeError::New(info.Env(), "Size cannot be negative");
        return;
    }

    SetHandle(av_buffer_pool_init(size, nullptr));
}

void NAVBufferPool::Free() {
    auto handle = GetHandle();
    av_buffer_pool_uninit(&handle);
    SetHandle(handle);
}

Napi::Value NAVBufferPool::Free(const Napi::CallbackInfo& info) {
    auto handle = GetHandle();
    av_buffer_pool_uninit(&handle);
    SetHandle(handle);
    return info.Env().Undefined();
}

Napi::Value NAVBufferPool::Get(const Napi::CallbackInfo& info) {
    auto handle = GetHandle();
    if (!handle) {
        Napi::TypeError::New(info.Env(), "Cannot retrieve a new buffer from a pool which has been marked as freed.").ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    auto buffer = av_buffer_pool_get(handle);
    if (!buffer) {
        Napi::TypeError::New(info.Env(), "Failed to get buffer from buffer pool");
        return info.Env().Undefined();
    }

    return NAVBuffer::FromHandleWrapped(info.Env(), buffer, true);
}

Napi::Value NAVBufferPool::IsFreed(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), !GetHandle());
}
