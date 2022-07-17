#include "channel-layout.h"

NAVChannelLayout::NAVChannelLayout(const Napi::CallbackInfo& info):
    NAVResource(info)
{
    if (ConstructFromHandle(info))
        return;
}

void NAVChannelLayout::Free() {
    // Do nothing. We don't own these.
}


Napi::Value NAVChannelLayout::GetOrder(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->order);
}

Napi::Value NAVChannelLayout::GetCount(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->nb_channels);
}
