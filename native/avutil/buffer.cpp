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
        ownedArrayBuffer = Napi::Reference<Napi::ArrayBuffer>::New(
            Napi::ArrayBuffer::New(callback.Env(), this->handle->data, this->handle->size),
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

        handle = buf_ref;
        LibAvAddon::Self(callback.Env())->RegisterResource(buf_ref->buffer, this);

        if (!ownedArrayBuffer.IsEmpty())
            ownedArrayBuffer.Unref();
        ownedArrayBuffer = Napi::Reference<Napi::ArrayBuffer>::New(
            Napi::ArrayBuffer::New(callback.Env(), buf_ref->data, buf_ref->size),
            1
        );
    } else if (callback[0].IsExternal()) {
        // This path should only ever be hit from NAVBuffer::FromHandle().
        handle = callback[0].As<Napi::External<AVBufferRef>>().Data();
        
        if (!ownedArrayBuffer.IsEmpty())
            ownedArrayBuffer.Unref();
        ownedArrayBuffer = Napi::Reference<Napi::ArrayBuffer>::New(
            Napi::ArrayBuffer::New(callback.Env(), this->handle->data, this->handle->size),
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
        handle = av_buffer_create((uint8_t*)buffer.Data(), buffer.ByteLength(), &NAVBuffer::Disown, this, 0);

        LibAvAddon::Self(callback.Env())->RegisterResource(handle->buffer, this);

    } else {
        Napi::TypeError::New(callback.Env(), "Invalid invocation").ThrowAsJavaScriptException();
        return;
    }
}

void NAVBuffer::Free() {
    av_buffer_unref(&handle);
}

void NAVBuffer::RefHandle() {
    handle = av_buffer_ref(handle);
}

void NAVBuffer::Disown(void *opaque, uint8_t *data) {
    NAVBuffer *buffer = (NAVBuffer*)opaque;
    if (!buffer->ownedArrayBuffer.IsEmpty())
        buffer->ownedArrayBuffer.Unref();
    buffer->ownedArrayBuffer = Napi::Reference<Napi::ArrayBuffer>();
    buffer->handle = nullptr;
}

Napi::Value NAVBuffer::GetRefCount(const Napi::CallbackInfo& info) {
    if (!this->handle)
        return Napi::Number::New(info.Env(), 0);
    
    return Napi::Number::New(info.Env(), av_buffer_get_ref_count(this->handle));
}

Napi::Value NAVBuffer::GetIsWritable(const Napi::CallbackInfo& info) {
    if (!this->handle)
        return Napi::Boolean::New(info.Env(), false);
    
    return Napi::Boolean::New(info.Env(), av_buffer_is_writable(this->handle) == 1);
}

Napi::Value NAVBuffer::MakeWritable(const Napi::CallbackInfo& info) {
    if (!this->handle) {
        Napi::TypeError::New(info.Env(), "Cannot make buffer writable after it has been freed").ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    if (av_buffer_is_writable(this->handle))
        return info.Env().Undefined();
    
    if (av_buffer_make_writable(&this->handle) < 0) {
        Napi::TypeError::New(info.Env(), "Failure while making buffer writable").ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    if (!ownedArrayBuffer.IsEmpty())
        ownedArrayBuffer.Unref();
    ownedArrayBuffer = Napi::Reference<Napi::ArrayBuffer>();
    return info.Env().Undefined();
}

Napi::Value NAVBuffer::Realloc(const Napi::CallbackInfo& info) {
    auto size = info[0].As<Napi::Number>().Int64Value();
    auto handle = this->handle;

    if (size < 0) {
        Napi::TypeError::New(info.Env(), "Size cannot be negative").ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }
    
    if (av_buffer_realloc(&this->handle, size) < 0) {
        Napi::TypeError::New(info.Env(), "Error while reallocating").ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    if (!ownedArrayBuffer.IsEmpty())
        ownedArrayBuffer.Unref();
    ownedArrayBuffer = Napi::Reference<Napi::ArrayBuffer>::New(Napi::ArrayBuffer::New(info.Env(), handle->data, handle->size), 1);
    return info.Env().Undefined();
}

Napi::Value NAVBuffer::Replace(const Napi::CallbackInfo& info) {
    auto other = Napi::ObjectWrap<NAVBuffer>::Unwrap(info[0].As<Napi::Object>());

    if (this == other || this->handle->buffer == other->handle->buffer)
        return info.Env().Undefined();
    
    av_buffer_replace(&this->handle, other->handle);
    if (!ownedArrayBuffer.IsEmpty())
        ownedArrayBuffer.Unref();
    ownedArrayBuffer = Napi::Reference<Napi::ArrayBuffer>();
    return info.Env().Undefined();
}


Napi::Value NAVBuffer::GetSize(const Napi::CallbackInfo& info) {
    if (!this->handle) {
        Napi::TypeError::New(info.Env(), "Cannot access size after buffer has been freed.");
        return info.Env().Undefined();
    }

    return Napi::Number::New(info.Env(), this->handle->size);
}

Napi::Value NAVBuffer::GetData(const Napi::CallbackInfo& info) {
    if (!this->handle) {
        Napi::TypeError::New(info.Env(), "Cannot access data after buffer has been freed.").ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }
    
    return ownedArrayBuffer.Value();
}

Napi::Value NAVBuffer::Free(const Napi::CallbackInfo& info) {
    if (this->handle)
        av_buffer_unref(&this->handle);
        
    if (!ownedArrayBuffer.IsEmpty())
        this->ownedArrayBuffer.Unref();
    this->ownedArrayBuffer = Napi::Reference<Napi::ArrayBuffer>();
    return info.Env().Undefined();
}

void NAVBufferPool::Init(Napi::Env env, Napi::Object exports) {
    auto func = DefineClass(env, "AVBufferPool", {
        InstanceAccessor("freed", &NAVBufferPool::IsFreed, nullptr),
        InstanceMethod<&NAVBufferPool::Free>("free"),
        InstanceMethod<&NAVBufferPool::Get>("get")
    });

    auto constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    
    LibAvAddon::Self(env)->AVBufferPool = constructor;
    exports.Set("AVBufferPool", constructor->Value());
}

NAVBufferPool::NAVBufferPool(const Napi::CallbackInfo& info):
    Napi::ObjectWrap<NAVBufferPool>(info)
{
    auto size = info[0].As<Napi::Number>().Int64Value();
    
    if (size < 0) {
        Napi::TypeError::New(info.Env(), "Size cannot be negative");
        return;
    }

    this->handle = av_buffer_pool_init(size, nullptr);
}

void NAVBufferPool::Finalize(Napi::Env env) {
    if (this->handle)
        av_buffer_pool_uninit(&this->handle);
}

Napi::Value NAVBufferPool::Free(const Napi::CallbackInfo& info) {
    av_buffer_pool_uninit(&this->handle);
    return info.Env().Undefined();
}

Napi::Value NAVBufferPool::Get(const Napi::CallbackInfo& info) {
    if (this->handle == nullptr) {
        Napi::TypeError::New(info.Env(), "Cannot retrieve a new buffer from a pool which has been marked as freed.").ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    auto buffer = av_buffer_pool_get(this->handle);
    if (!buffer) {
        Napi::TypeError::New(info.Env(), "Failed to get buffer from buffer pool");
        return info.Env().Undefined();
    }

    return NAVBuffer::FromHandleWrapped(info.Env(), buffer, true);
}

Napi::Value NAVBufferPool::IsFreed(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), handle == nullptr);
}
