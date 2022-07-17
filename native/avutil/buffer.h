#include <napi.h>

#include "../common.h"
#include "../resource.h"

extern "C" {
    #include <libavutil/buffer.h>
}

void *GetRegisterableBufferHandle(void *bufferRef);

class NAVBuffer : public NAVResource<NAVBuffer, AVBufferRef, GetRegisterableBufferHandle> {
    public:
        NAVBuffer(const Napi::CallbackInfo& info);

        inline static std::string ExportName() { return "AVBuffer"; }
        inline static Napi::Function ClassDefinition(const Napi::Env &env) {
            return DefineClass(env, "AVBuffer", {
                R_GETTER("size", &NAVBuffer::GetSize),
                R_GETTER("data", &NAVBuffer::GetData),
                R_GETTER("refCount", &NAVBuffer::GetRefCount),
                R_GETTER("writable", &NAVBuffer::GetIsWritable),
                R_METHOD("free", &NAVBuffer::Free),
                R_METHOD("makeWritable", &NAVBuffer::MakeWritable),
                R_METHOD("realloc", &NAVBuffer::Realloc)
            });
        }

        virtual void Free();
        virtual void RefHandle();

    private:
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

class NAVBufferPool : public NAVResource<NAVBufferPool, AVBufferPool> {
    public:
        NAVBufferPool(const Napi::CallbackInfo& info);
        
        inline static std::string ExportName() { return "AVBufferPool"; }
        inline static Napi::Function ClassDefinition(const Napi::Env &env) {
            return DefineClass(env, "AVBufferPool", {
                R_GETTER("freed", &NAVBufferPool::IsFreed),
                R_METHOD("free", &NAVBufferPool::Free),
                R_METHOD("get", &NAVBufferPool::Get)
            });
        }
        
        virtual void Free();
    private:
        Napi::Value Free(const Napi::CallbackInfo& info);
        Napi::Value Get(const Napi::CallbackInfo& info);
        Napi::Value IsFreed(const Napi::CallbackInfo& info);
};
