#include "../libav.h"
#include "dict.h"

NAVDictionary::NAVDictionary(const Napi::CallbackInfo& info):
    Napi::ObjectWrap<NAVDictionary>(info)
{
    handle = nullptr;
}

void NAVDictionary::Init(Napi::Env env, Napi::Object exports) {
    auto func = DefineClass(env, "AVDictionary", {
        InstanceAccessor("count", &NAVDictionary::Count, nullptr),
        InstanceAccessor("keys", &NAVDictionary::GetKeys, nullptr),
        InstanceMethod<&NAVDictionary::Get>("get"),
        InstanceMethod<&NAVDictionary::Set>("set"),
        InstanceMethod<&NAVDictionary::Copy>("copy"),
        InstanceMethod<&NAVDictionary::Clear>("clear"),
    });

    auto constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    
    LibAvAddon::Self(env)->AVDictionary = constructor;
    exports.Set("AVDictionary", constructor->Value());
}

void NAVDictionary::Finalize(Napi::Env env) {
    av_dict_free(&handle);
}


Napi::Value NAVDictionary::Get(const Napi::CallbackInfo& info) {
    auto key = info[0].ToString().Utf8Value();
    AVDictionaryEntry *prev = nullptr;
    uint32_t flags = 0;

    if (info.Length() > 1) {
        if (!info[1].IsUndefined() && !info[1].IsNull()) {
            auto nPrev = NAVDictionaryEntry::Unwrap(info[1].As<Napi::Object>());
            if (nPrev)
                prev = nPrev->handle;
        }

        if (info.Length() > 2) {
            flags = info[2].ToNumber().Uint32Value();
        }
    }

    auto entry = av_dict_get(handle, key.c_str(), prev, flags);
    if (!entry)
        return info.Env().Undefined();
    
    return LibAvAddon::Self(info.Env())->AVDictionaryEntry->New({
        Napi::External<AVDictionaryEntry>::New(info.Env(), entry)
    });
}

Napi::Value NAVDictionary::Count(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), av_dict_count(this->handle));
}

Napi::Value NAVDictionary::Set(const Napi::CallbackInfo& info) {
    auto key = info[0].As<Napi::String>().Utf8Value();
    std::string valueStr = "";
    
    if (info.Length() > 0 && !info[1].IsNull() && !info[1].IsUndefined())
        valueStr = info[1].ToString().Utf8Value();
    
    uint32_t flags = 0;

    if (info.Length() > 2)
        flags = info[2].ToNumber().Uint32Value();

    auto status = av_dict_set(
        &this->handle, key.c_str(), 
        info[1].IsNull() || info[1].IsUndefined() 
            ? nullptr 
            : valueStr.c_str(), 
        flags
    );

    if (status < 0) {
        Napi::Error::New(info.Env(), "Failed to add dictionary key: " + status)
            .ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    return Napi::Boolean::New(info.Env(), true);
}

Napi::Value NAVDictionary::Copy(const Napi::CallbackInfo& info) {
    auto other = Unwrap(info[0].As<Napi::Object>());
    uint32_t flags = 0;
    if (info.Length() > 1)
        flags = info[1].ToNumber().Uint32Value();

    if (!other) {
        Napi::TypeError::New(info.Env(), "Invalid target dictionary")
            .ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    auto status = av_dict_copy(&other->handle, handle, flags);

    if (status < 0) {
        Napi::Error::New(info.Env(), "Failed to copy dictionary: " + status)
            .ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    return Napi::Boolean::New(info.Env(), true);
}

Napi::Value NAVDictionary::Clear(const Napi::CallbackInfo& info) {
    av_dict_free(&handle);
    return info.Env().Undefined();
}

Napi::Value NAVDictionary::GetKeys(const Napi::CallbackInfo& info) {
    std::vector<std::string> keys;
    AVDictionaryEntry *entry = nullptr;

    while (entry = av_dict_get(handle, "", entry, AV_DICT_IGNORE_SUFFIX))
        keys.push_back(entry->key);

    auto array = Napi::Array::New(info.Env(), keys.size());
    for (int i = 0, max = keys.size(); i < max; ++i)
        array.Set(i, keys[i]);
    return array;
}


NAVDictionaryEntry::NAVDictionaryEntry(const Napi::CallbackInfo& info):
    Napi::ObjectWrap<NAVDictionaryEntry>(info)
{
    handle = info[0].As<Napi::External<AVDictionaryEntry>>().Data(); 
    key = handle->key;
    value = handle->value;
}

void NAVDictionaryEntry::Init(Napi::Env env, Napi::Object exports) {
    auto func = DefineClass(env, "AVDictionaryEntry", {
        InstanceAccessor("key", &NAVDictionaryEntry::GetKey, nullptr),
        InstanceAccessor("value", &NAVDictionaryEntry::GetValue, nullptr),
        InstanceMethod<&NAVDictionaryEntry::ToJSON>("toJSON"),
        InstanceMethod<&NAVDictionaryEntry::ToString>("toString")
    });

    auto constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    
    LibAvAddon::Self(env)->AVDictionaryEntry = constructor;
    exports.Set("AVDictionaryEntry", constructor->Value());

}

Napi::Value NAVDictionaryEntry::GetKey(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), key);
}

Napi::Value NAVDictionaryEntry::GetValue(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), value);
}

Napi::Value NAVDictionaryEntry::ToJSON(const Napi::CallbackInfo& info) {
    auto obj = Napi::Object::New(info.Env());

    obj.Set("key", GetKey(info));
    obj.Set("value", GetValue(info));

    return obj;
}

Napi::Value NAVDictionaryEntry::ToString(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), "AVDictionaryEntry { key: " + key + ", value: " + value + " }");
}
