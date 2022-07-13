#include <libavutil/avutil.h>

#include "buffer.h"
#include "../libav.h"
#include "../helpers.h"

NAVBuffer::NAVBuffer(const Napi::CallbackInfo &callback):
    Napi::ObjectWrap<NAVBuffer>(callback)
{
    AVBufferRef *handle;
    
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

        this->ownedArrayBuffer.Unref();
        this->ownedArrayBuffer = Napi::Reference<Napi::ArrayBuffer>::New(
            Napi::ArrayBuffer::New(callback.Env(), buf_ref->data, buf_ref->size),
            1
        );
        this->handle = buf_ref;
    } else if (callback[0].IsExternal()) {
        handle = callback[0].As<Napi::External<AVBufferRef>>().Data();
        this->handle = handle;
    } else if (callback[0].IsTypedArray() || callback[0].IsArrayBuffer()) {
        Napi::ArrayBuffer buffer;

        if (callback[0].IsTypedArray())
            buffer = callback[0].As<Napi::TypedArray>().ArrayBuffer();
        else if (callback[0].IsBuffer())
            buffer = callback[0].As<Napi::Buffer<uint8_t>>().ArrayBuffer();
        else
            buffer = callback[0].As<Napi::ArrayBuffer>();

        this->ownedArrayBuffer.Unref();
        this->ownedArrayBuffer = Napi::Reference<Napi::ArrayBuffer>::New(buffer, 1);
        this->handle = av_buffer_create((uint8_t*)buffer.Data(), buffer.ByteLength(), &NAVBuffer::Disown, this, 0);
    } else {
        Napi::TypeError::New(callback.Env(), "Invalid invocation").ThrowAsJavaScriptException();
        return;
    }
}

void NAVBuffer::Disown(void *opaque, uint8_t *data) {
    NAVBuffer *buffer = (NAVBuffer*)opaque;
    buffer->ownedArrayBuffer.Unref();
    buffer->ownedArrayBuffer = Napi::Reference<Napi::ArrayBuffer>();
    buffer->handle = nullptr;
}

void NAVBuffer::Init(Napi::Env env, Napi::Object exports) {
    auto func = DefineClass(env, "AVBuffer", {
        InstanceAccessor("size", &NAVBuffer::GetSize, nullptr),
        InstanceAccessor("data", &NAVBuffer::GetData, nullptr),
        InstanceAccessor("refCount", &NAVBuffer::GetRefCount, nullptr),
        InstanceAccessor("writable", &NAVBuffer::GetIsWritable, nullptr),
        InstanceMethod<&NAVBuffer::Free>("free"),
        StaticMethod<&NAVBuffer::Alloc>("alloc")
    });

    auto constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    
    LibAvAddon::Self(env)->AVBuffer = constructor;

    exports.Set("NAVBufferRef", constructor);
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

Napi::Value NAVBuffer::GetSize(const Napi::CallbackInfo& info) {
    if (!this->handle) {
        Napi::TypeError::New(info.Env(), "Cannot access buffer size after buffer has been freed.");
        return info.Env().Undefined();
    }

    return Napi::Number::New(info.Env(), this->handle->size);
}

Napi::Value NAVBuffer::GetData(const Napi::CallbackInfo& info) {
    if (!this->handle) {
        Napi::TypeError::New(info.Env(), "Cannot access buffer after buffer has been freed.");
        return info.Env().Undefined();
    }
    
    return Napi::Buffer<uint8_t>::New(info.Env(), this->handle->data, this->handle->size);
}

Napi::Value NAVBuffer::Free(const Napi::CallbackInfo& info) {
    if (this->handle)
        av_buffer_unref(&this->handle);
        
    this->ownedArrayBuffer.Unref();
    this->ownedArrayBuffer = Napi::Reference<Napi::ArrayBuffer>();
}

void NAVBuffer::Finalize(Napi::Env env) {
    if (this->handle) {
        av_buffer_unref(&this->handle);
    }
}

Napi::Value NAVBuffer::Alloc(const Napi::CallbackInfo& info) {
    auto size = info[0].As<Napi::Number>().Int64Value();
    auto zero = false;

    if (info.Length() > 1)
        zero = info[1].As<Napi::Boolean>().Value();

    if (size < 0) {
        Napi::TypeError::New(info.Env(), "Size cannot be negative").ThrowAsJavaScriptException();
        return;
    }

    AVBufferRef *buf_ref;
    
    if (zero)
        buf_ref = av_buffer_allocz(size);
    else
        buf_ref = av_buffer_alloc(size);

    if (!buf_ref) {
        Napi::TypeError::New(info.Env(), "Failed to allocate buffer").ThrowAsJavaScriptException();
        return;
    }

    return LibAvAddon::Self(info.Env())->AVBuffer->New({ Napi::External<AVBufferRef>::New(info.Env(), buf_ref) });
}

Napi::Value nlavu_av_buffer_create(const Napi::CallbackInfo& callback);
Napi::Value nlavu_av_buffer_default_free(const Napi::CallbackInfo& callback);
Napi::Value nlavu_av_buffer_ref(const Napi::CallbackInfo& callback);
Napi::Value nlavu_av_buffer_unref(const Napi::CallbackInfo& callback);
Napi::Value nlavu_av_buffer_is_writable(const Napi::CallbackInfo& callback);
Napi::Value nlavu_av_buffer_get_opaque(const Napi::CallbackInfo& callback);
Napi::Value nlavu_av_buffer_get_ref_count(const Napi::CallbackInfo& callback);
Napi::Value nlavu_av_buffer_make_writable(const Napi::CallbackInfo& callback);
Napi::Value nlavu_av_buffer_realloc(const Napi::CallbackInfo& callback);
Napi::Value nlavu_av_buffer_replace(const Napi::CallbackInfo& callback);
Napi::Value nlavu_av_buffer_pool_init(const Napi::CallbackInfo& callback);
Napi::Value nlavu_av_buffer_pool_init2(const Napi::CallbackInfo& callback);
Napi::Value nlavu_av_buffer_pool_uninit(const Napi::CallbackInfo& callback);
Napi::Value nlavu_av_buffer_pool_get(const Napi::CallbackInfo& callback);
Napi::Value nlavu_av_buffer_pool_buffer_get_opaque(const Napi::CallbackInfo& callback);
