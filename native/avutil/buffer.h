#include <napi.h>
#include <libavutil/buffer.h>

class NAVBuffer : public Napi::ObjectWrap<NAVBuffer> {
    public:
        NAVBuffer(const Napi::CallbackInfo& info);
        static void Init(Napi::Env env, Napi::Object exports);
        static Napi::Value Alloc(const Napi::CallbackInfo& info);
        
        virtual void Finalize(Napi::Env env);
    private:
        AVBufferRef *handle;
        Napi::Reference<Napi::ArrayBuffer> ownedArrayBuffer;
        Napi::Value GetSize(const Napi::CallbackInfo& info);
        Napi::Value GetData(const Napi::CallbackInfo& info);
        Napi::Value Free(const Napi::CallbackInfo& info);
        Napi::Value GetRefCount(const Napi::CallbackInfo& info);
        Napi::Value GetIsWritable(const Napi::CallbackInfo& info);

        /**
         * Callback for av_buffer_create to handle freeing an owned ArrayBuffer
         */
        static void Disown(void *opaque, uint8_t *data);
};

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
