#include "../common.h"

#include <napi.h>

extern "C" {
    #include <libavutil/dict.h>
}

class NAVDictionary : public Napi::ObjectWrap<NAVDictionary> {
    public:
        NAVDictionary(const Napi::CallbackInfo& info);
        static void Init(Napi::Env env, Napi::Object exports);
        virtual void Finalize(Napi::Env env);
    private:
        AVDictionary *handle;

        Napi::Value Count(const Napi::CallbackInfo& info);
        Napi::Value Get(const Napi::CallbackInfo& info);
        Napi::Value Set(const Napi::CallbackInfo& info);
        Napi::Value Copy(const Napi::CallbackInfo& info);
        Napi::Value Clear(const Napi::CallbackInfo& info);
        Napi::Value GetKeys(const Napi::CallbackInfo& info);
};

class NAVDictionaryEntry : public Napi::ObjectWrap<NAVDictionaryEntry> {
    public:
        NAVDictionaryEntry(const Napi::CallbackInfo& info);
        static void Init(Napi::Env env, Napi::Object exports);
        AVDictionaryEntry *handle = nullptr;

    private:
        std::string key;
        std::string value;

        Napi::Value GetKey(const Napi::CallbackInfo& info);
        Napi::Value GetValue(const Napi::CallbackInfo& info);
        Napi::Value ToJSON(const Napi::CallbackInfo& info);
        Napi::Value ToString(const Napi::CallbackInfo& info);
};