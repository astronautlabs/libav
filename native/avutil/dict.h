#include "../common.h"

#include <napi.h>
#include "../resource.h"

extern "C" {
    #include <libavutil/dict.h>
}

class NAVDictionary : public NAVResource<NAVDictionary, AVDictionary> {
    public:
        NAVDictionary(const Napi::CallbackInfo& info);
        
        inline static std::string ExportName() { return "AVDictionary"; }
        inline static Napi::Function ClassDefinition(const Napi::Env &env) {
            return DefineClass(env, "AVDictionary", {
                InstanceAccessor("count", &NAVDictionary::Count, nullptr),
                InstanceAccessor("keys", &NAVDictionary::GetKeys, nullptr),
                InstanceMethod<&NAVDictionary::Get>("get"),
                InstanceMethod<&NAVDictionary::Set>("set"),
                InstanceMethod<&NAVDictionary::CopyTo>("copyTo"),
                InstanceMethod<&NAVDictionary::Clear>("clear"),
            });
        }

        virtual void Free();
    private:
        Napi::Value Count(const Napi::CallbackInfo& info);
        Napi::Value Get(const Napi::CallbackInfo& info);
        Napi::Value Set(const Napi::CallbackInfo& info);
        Napi::Value CopyTo(const Napi::CallbackInfo& info);
        Napi::Value Clear(const Napi::CallbackInfo& info);
        Napi::Value GetKeys(const Napi::CallbackInfo& info);
};

class NAVDictionaryEntry : public NAVResource<NAVDictionaryEntry, AVDictionaryEntry> {
    public:
        NAVDictionaryEntry(const Napi::CallbackInfo& info);

        inline static std::string ExportName() { return "AVDictionaryEntry"; }
        inline static Napi::Function ClassDefinition(const Napi::Env &env) {
            return DefineClass(env, "AVDictionaryEntry", {
                InstanceAccessor("key", &NAVDictionaryEntry::GetKey, nullptr),
                InstanceAccessor("value", &NAVDictionaryEntry::GetValue, nullptr),
                InstanceMethod<&NAVDictionaryEntry::ToJSON>("toJSON"),
                InstanceMethod<&NAVDictionaryEntry::ToString>("toString")
            });
        }

        void Free();
        virtual bool IsResourceMappingEnabled();
    private:
        std::string key;
        std::string value;

        Napi::Value GetKey(const Napi::CallbackInfo& info);
        Napi::Value GetValue(const Napi::CallbackInfo& info);
        Napi::Value ToJSON(const Napi::CallbackInfo& info);
        Napi::Value ToString(const Napi::CallbackInfo& info);
};