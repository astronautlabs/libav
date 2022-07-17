#include "codec.h"
#include "../helpers.h"
#include "../avutil/class.h"
#include "profile.h"
#include "../avutil/channel-layout.h"

NAVCodec::NAVCodec(const Napi::CallbackInfo& info):
    NAVResource(info)
{
    if (ConstructFromHandle(info))
        return;
}

void NAVCodec::Free() {
    // Do nothing. We never own codecs.
}

Napi::Value NAVCodec::GetVersion(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), avcodec_version());
}

Napi::Value NAVCodec::GetConfiguration(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), avcodec_configuration());
}

Napi::Value NAVCodec::GetLicense(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), avcodec_license());
}


Napi::Value NAVCodec::All(const Napi::CallbackInfo& info) {
    void *iter = nullptr;
    std::vector<Napi::Value> vec;

    while (auto codec = av_codec_iterate(&iter)) {
        vec.push_back(NAVCodec::FromHandleWrapped(info.Env(), (AVCodec*)codec, false));
    }

    return VectorToArray(info.Env(), vec);
}

Napi::Value NAVCodec::FindDecoder(const Napi::CallbackInfo& info) {
    if (info[0].IsString()) {
        return NAVCodec::FromHandleWrapped(
            info.Env(),
            (AVCodec*)avcodec_find_decoder_by_name(info[0].ToString().Utf8Value().c_str()),
            false
        );
    } else if (info[0].IsNumber()) {
        return NAVCodec::FromHandleWrapped(
            info.Env(),
            (AVCodec*)avcodec_find_decoder((AVCodecID)info[0].ToNumber().Int32Value()),
            false
        );
    }

    return info.Env().Undefined();
}

Napi::Value NAVCodec::FindEncoder(const Napi::CallbackInfo& info) {
    if (info[0].IsString()) {
        return NAVCodec::FromHandleWrapped(
            info.Env(),
            (AVCodec*)avcodec_find_encoder_by_name(info[0].ToString().Utf8Value().c_str()),
            false
        );
    } else if (info[0].IsNumber()) {
        return NAVCodec::FromHandleWrapped(
            info.Env(),
            (AVCodec*)avcodec_find_encoder((AVCodecID)info[0].ToNumber().Int32Value()),
            false
        );
    }

    return info.Env().Undefined();
}

Napi::Value NAVCodec::GetName(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), GetHandle()->name);
}

Napi::Value NAVCodec::GetLongName(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), GetHandle()->long_name);
}

Napi::Value NAVCodec::GetType(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->type);
}

Napi::Value NAVCodec::GetId(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->id);
}

Napi::Value NAVCodec::GetCapabilities(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->capabilities);
}

Napi::Value NAVCodec::GetDecoderMaxLowResolution(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), GetHandle()->max_lowres);
}

Napi::Value NAVCodec::GetSupportedFrameRates(const Napi::CallbackInfo& info) {
    return NRationalArrayZeroTerminated(info.Env(), GetHandle()->supported_framerates);
}

Napi::Value NAVCodec::GetPixelFormats(const Napi::CallbackInfo& info) {
    return VectorToArray(
        info.Env(), 
        Transform<int, Napi::Value>(
            TerminatedArray((int*)GetHandle()->pix_fmts, -1),
            WrapNumbers<int>(info.Env())
        )
    );
}

Napi::Value NAVCodec::GetSupportedSampleRates(const Napi::CallbackInfo& info) {
    return VectorToArray(
        info.Env(), 
        Transform<int, Napi::Value>(
            TerminatedArray(GetHandle()->supported_samplerates, 0),
            WrapNumbers<int>(info.Env())
        )
    );
}

Napi::Value NAVCodec::GetSampleFormats(const Napi::CallbackInfo& info) {
    return VectorToArray(
        info.Env(), 
        Transform<AVSampleFormat, Napi::Value>(
            TerminatedArray(GetHandle()->sample_fmts, (AVSampleFormat)-1),
            WrapNumbers<AVSampleFormat>(info.Env())
        )
    );
}

Napi::Value NAVCodec::GetPrivateClass(const Napi::CallbackInfo& info) {
    return NAVClass::FromHandleWrapped(info.Env(), (AVClass*)GetHandle()->priv_class, false);
}

Napi::Value NAVCodec::GetProfiles(const Napi::CallbackInfo& info) {
    std::vector<const AVProfile*> vec;
    const AVProfile *profiles = GetHandle()->profiles;
    while (profiles->profile != FF_PROFILE_UNKNOWN)
        vec.push_back(profiles++);
    
    return VectorToArray(
        info.Env(),
        Transform<const AVProfile*, Napi::Value>(
            vec, 
            [&](const AVProfile *profile) { 
                return (Napi::Value)NAVProfile::FromHandleWrapped(info.Env(), (AVProfile*)profile, false); 
            }
        )
    );
}

Napi::Value NAVCodec::GetWrapperName(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), GetHandle()->wrapper_name);
}

Napi::Value NAVCodec::GetChannelLayouts(const Napi::CallbackInfo& info) {
    std::vector <Napi::Value> vec;
    auto layouts = GetHandle()->ch_layouts;

    while (layouts->order != 0 || layouts->nb_channels != 0) {
        vec.push_back(NAVChannelLayout::FromHandle(info.Env(), (AVChannelLayout*)layouts++, false)->Value());
    }

    return VectorToArray(info.Env(), vec);
}

Napi::Value NAVCodec::GetIsEncoder(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), av_codec_is_encoder(GetHandle()));
}

Napi::Value NAVCodec::GetIsDecoder(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), av_codec_is_decoder(GetHandle()));
}

Napi::Value NAVCodec::GetProfileName(const Napi::CallbackInfo& info) {
    int profile = info[0].As<Napi::Number>().Int32Value();
    return Napi::String::New(info.Env(), av_get_profile_name(GetHandle(), profile));
}
