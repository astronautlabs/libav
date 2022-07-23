#include "channel-layout.h"

#ifdef FFMPEG_5_1

NAVChannelLayout::NAVChannelLayout(const Napi::CallbackInfo& info):
    NAVResource(info)
{
    if (ConstructFromHandle(info))
        return;
}

void NAVChannelLayout::Free() {
    if (owned) {
        auto handle = GetHandle();
        av_free(handle);
        SetHandle(nullptr);
    }
}

void NAVChannelLayout::RefBuffer() {
    owned = false;
}

Napi::Value NAVChannelLayout::FromMask(const Napi::CallbackInfo& info) {
    bool truncated;
    uint64_t mask = info[0].As<Napi::BigInt>().Uint64Value(&truncated);

    if (truncated) {
        Napi::TypeError::New(info.Env(), "The mask is too large to fit in a 64-bit unsigned integer.")
            .ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    AVChannelLayout *layout = (AVChannelLayout*)av_malloc(sizeof(AVChannelLayout));
    av_channel_layout_from_mask(layout, mask);
    return NAVChannelLayout::FromHandleWrapped(info.Env(), layout, true);
}

Napi::Value NAVChannelLayout::FromString(const Napi::CallbackInfo& info) {
    auto str = info[0].ToString().Utf8Value();
    AVChannelLayout *layout = (AVChannelLayout*)av_malloc(sizeof(AVChannelLayout));
    int result = av_channel_layout_from_string(layout, str.c_str());
    if (result < 0)
        return nlav_throw(info.Env(), result, "av_channel_layout_from_string");
    
    return NAVChannelLayout::FromHandleWrapped(info.Env(), layout, true);
}

Napi::Value NAVChannelLayout::FromDefault(const Napi::CallbackInfo& info) {
    int count = info[0].As<Napi::Number>().Int32Value();
    AVChannelLayout *layout = (AVChannelLayout*)av_malloc(sizeof(AVChannelLayout));
    av_channel_layout_default(layout, count);

    return NAVChannelLayout::FromHandleWrapped(info.Env(), layout, true);
}

Napi::Value NAVChannelLayout::GetStandards(const Napi::CallbackInfo& info) {
    void *iter = nullptr;
    std::vector<Napi::Value> vec;

    while (const AVChannelLayout *layout = av_channel_layout_standard(&iter))
        vec.push_back(NAVChannelLayout::FromHandleWrapped(info.Env(), (AVChannelLayout*)layout, false));

    return VectorToArray(info.Env(), vec);
}

// Methods

Napi::Value NAVChannelLayout::Uninit(const Napi::CallbackInfo& info) {
    av_channel_layout_uninit(GetHandle());
    return info.Env().Undefined();
}

// Properties

Napi::Value NAVChannelLayout::GetOrder(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->order);
}

void NAVChannelLayout::SetOrder(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->order = (AVChannelOrder)info[0].As<Napi::Number>().Int32Value();
}

Napi::Value NAVChannelLayout::GetCount(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->nb_channels);
}

void NAVChannelLayout::SetCount(const Napi::CallbackInfo& info, const Napi::Value& value) {
    GetHandle()->nb_channels = info[0].As<Napi::Number>().Int32Value();
}

Napi::Value NAVChannelLayout::GetMask(const Napi::CallbackInfo& info) {
    if (GetHandle()->order == AV_CHANNEL_ORDER_NATIVE || GetHandle()->order == AV_CHANNEL_ORDER_AMBISONIC) {
        return Napi::BigInt::New(info.Env(), GetHandle()->u.mask);
    }

    return Napi::BigInt::New(info.Env(), 0ULL);
}

void NAVChannelLayout::SetMask(const Napi::CallbackInfo& info, const Napi::Value& value) {
    if (GetHandle()->order != AV_CHANNEL_ORDER_NATIVE && GetHandle()->order != AV_CHANNEL_ORDER_AMBISONIC) {
        Napi::TypeError::New(
            info.Env(), 
            "The `mask` field is only valid for channel layouts with order "
            "AV_CHANNEL_ORDER_NATIVE or AV_CHANNEL_ORDER_AMBISONIC"
        );
        return;
    }

    bool truncated;
    GetHandle()->u.mask = value.As<Napi::BigInt>().Uint64Value(&truncated);
}

Napi::Value NAVChannelLayout::GetCustomOrder(const Napi::CallbackInfo& info) {
    auto handle = GetHandle();
    if (handle->order != AV_CHANNEL_ORDER_CUSTOM) {
        Napi::TypeError::New(
            info.Env(), 
            "The `customChannels` field is only valid for channel layouts with order AV_CHANNEL_ORDER_CUSTOM"
        );
        return info.Env().Undefined();
    }

    if (!handle->u.mask)
        return Napi::Array::New(info.Env());

    std::vector<Napi::Value> vec;

    for (int i = 0, max = handle->nb_channels; i < max; ++i) {
        Napi::Object obj = Napi::Object::New(info.Env());

        obj.Set("id", Napi::Number::New(info.Env(), handle->u.map[i].id));
        obj.Set("name", Napi::String::New(info.Env(), handle->u.map[i].name));
        
        vec.push_back(obj);
    }

    return VectorToArray(info.Env(), vec);
}

void NAVChannelLayout::SetCustomOrder(const Napi::CallbackInfo& info, const Napi::Value& value) {
    auto handle = GetHandle();
    if (handle->order != AV_CHANNEL_ORDER_CUSTOM) {
        Napi::TypeError::New(
            info.Env(), 
            "The `customChannels` field is only valid for channel layouts with order AV_CHANNEL_ORDER_CUSTOM"
        );
        return;
    }

    if (handle->u.map) {
        av_freep(&handle->u.map);
    }

    Napi::Array array = value.As<Napi::Array>();
    
    for (int i = 0, max = array.Length(); i < max; ++i) {
        Napi::Object obj = array.Get(i).As<Napi::Object>();
        
        if (obj.Get("name") == info.Env().Undefined() || obj.Get("name") == info.Env().Null())
            continue;

        Napi::String name = obj.Get("name").As<Napi::String>();
        if (name.Utf8Value().size() > 16) {
            Napi::TypeError::New(info.Env(), "Name field of value at index " + std::to_string(i) + " is too long (max 16 characters)")
                .ThrowAsJavaScriptException();
            return;
        }
    }

    auto block = (AVChannelCustom *)av_malloc(sizeof(AVChannelCustom) * array.Length());

    for (int i = 0, max = array.Length(); i < max; ++i) {
        Napi::Object obj = array.Get(i).As<Napi::Object>();
        block[i].id = (AVChannel)obj.Get("id").As<Napi::Number>().Int32Value();
        block[i].opaque = nullptr;
        memset(block[i].name, 0, 16);

        if (obj.Get("name") != info.Env().Undefined() && obj.Get("name") != info.Env().Null()) {
            Napi::String name = obj.Get("name").As<Napi::String>();
            name.Utf8Value().copy(block[i].name, 16);
        }
    }

    handle->u.map = block;
    handle->nb_channels = array.Length();
}

#endif