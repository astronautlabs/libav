#include "profile.h"

NAVProfile::NAVProfile(const Napi::CallbackInfo& info):
    NAVResource(info)
{
    if (ConstructFromHandle(info))
        return;
}

void NAVProfile::Free() {
    // Do nothing. We never own AVProfiles.
}

Napi::Value NAVProfile::GetId(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->profile);
}
Napi::Value NAVProfile::GetName(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), GetHandle()->name);
}