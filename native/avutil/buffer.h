#include "../common.h"

#include <napi.h>

extern "C" {
    #include <libavutil/buffer.h>
}

class NAVBuffer : public Napi::ObjectWrap<NAVBuffer> {
    public:
        NAVBuffer(const Napi::CallbackInfo& info);
        static void Init(Napi::Env env, Napi::Object exports);
        virtual void Finalize(Napi::Env env);
    private:
        AVBufferRef *handle;
        Napi::Reference<Napi::ArrayBuffer> ownedArrayBuffer;
        Napi::Value GetSize(const Napi::CallbackInfo& info);
        Napi::Value GetData(const Napi::CallbackInfo& info);
        Napi::Value Free(const Napi::CallbackInfo& info);
        Napi::Value GetRefCount(const Napi::CallbackInfo& info);
        Napi::Value GetIsWritable(const Napi::CallbackInfo& info);
        Napi::Value MakeWritable(const Napi::CallbackInfo& info);
        Napi::Value Realloc(const Napi::CallbackInfo& info);
        Napi::Value Replace(const Napi::CallbackInfo& info);

        /**
         * Callback for av_buffer_create to handle freeing an owned ArrayBuffer
         */
        static void Disown(void *opaque, uint8_t *data);
};

class NAVBufferPool : public Napi::ObjectWrap<NAVBufferPool> {
    public:
        NAVBufferPool(const Napi::CallbackInfo& info);
        static void Init(Napi::Env env, Napi::Object exports);
        virtual void Finalize(Napi::Env env);
    private:
        AVBufferPool *handle;
        Napi::Value Free(const Napi::CallbackInfo& info);
        Napi::Value Get(const Napi::CallbackInfo& info);
        Napi::Value IsFreed(const Napi::CallbackInfo& info);
};
