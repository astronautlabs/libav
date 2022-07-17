#include "class.h"

NAVClass::NAVClass(const Napi::CallbackInfo& info):
    NAVResource(info)
{
    if (ConstructFromHandle(info))
        return;
}

void NAVClass::Free() {
    // Do nothing, AVClass is static.
}

Napi::Value NAVClass::GetName(const Napi::CallbackInfo& info) {
    auto handle = GetHandle();
    return Napi::String::New(info.Env(), handle->class_name);
}

Napi::Value NAVClass::GetVersion(const Napi::CallbackInfo& info) {
    auto handle = GetHandle();
    return Napi::Number::New(info.Env(), handle->version);
}

Napi::Value NAVClass::GetCategory(const Napi::CallbackInfo& info) {
    auto handle = GetHandle();
    return Napi::Number::New(info.Env(), handle->category);
}

Napi::Value NAVClass::GetContextName(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), GetHandle()->item_name(
        info[0].As<Napi::External<void>>().Data()
    ));
}

Napi::Value NAVClass::GetContextCategory(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->get_category(
        info[0].As<Napi::External<void>>().Data()
    ));
}



