#include "../common.h"

extern "C" {
    #include <libavutil/avutil.h>
    #include <libavutil/channel_layout.h>
}

#include "avutil.h"
#include "../helpers.h"

NAVUtil::NAVUtil(const Napi::CallbackInfo& info):
    NAVResource(info)
{
    Napi::TypeError::New(info.Env(), "This class cannot be constructed.").ThrowAsJavaScriptException();
}

void NAVUtil::Free() {
    // Do nothing. NAVUtil does not have a handle.
}

Napi::Value NAVUtil::GetVersion(const Napi::CallbackInfo& callback) {
    return Napi::Number::New(callback.Env(), avutil_version());
}

Napi::Value NAVUtil::GetVersionInfo(const Napi::CallbackInfo& callback) {
    return Napi::String::New(callback.Env(), av_version_info());
}

Napi::Value NAVUtil::GetConfiguration(const Napi::CallbackInfo& callback) {
    return Napi::String::New(callback.Env(), avutil_configuration());
}

Napi::Value NAVUtil::GetLicense(const Napi::CallbackInfo& callback) {
    return Napi::String::New(callback.Env(), avutil_license());
}

Napi::Value NAVUtil::GetMediaTypeString(const Napi::CallbackInfo& callback) {
    auto media_type = callback[0].As<Napi::Number>().Int32Value();
    return Napi::String::New(callback.Env(), av_get_media_type_string((AVMediaType)media_type));
}

Napi::Value NAVUtil::GetPictureTypeString(const Napi::CallbackInfo& callback) {
    auto pict_type = callback[0].As<Napi::Number>().Int32Value();
    char buf[2];

    buf[1] = 0;
    buf[0] = av_get_picture_type_char((AVPictureType) pict_type);
    return Napi::String::New(callback.Env(), (const char *)&buf);
}

Napi::Value NAVUtil::GetTimeBase(const Napi::CallbackInfo& callback) {
    return NRational(callback.Env(), av_get_time_base_q());

}

Napi::Value NAVUtil::GetFourCCString(const Napi::CallbackInfo& callback) {
    auto fourcc = callback[0].As<Napi::Number>().Uint32Value();
    char buf[AV_FOURCC_MAX_STRING_SIZE];

    av_fourcc_make_string((char *)&buf, fourcc);

    return Napi::String::New(callback.Env(), (char *)buf);
}

Napi::Value NAVUtil::GetChannelName(const Napi::CallbackInfo& callback) {
#ifdef FFMPEG_5_1
    auto channel = (AVChannel) callback[0].As<Napi::Number>().Int32Value();
    char buf[128];
    memset(buf, 0, 128);
    int length = av_get_channel_name(buf, 127, channel);
    
    if (length < 0)
        nlav_throw(callback.Env(), length, "av_channel_name");
    
    return Napi::String::New(callback.Env(), buf);
#else
    auto channel = callback[0].As<Napi::Number>().Int32Value();
    return Napi::String::New(callback.Env(), av_get_channel_name(channel));
#endif
}

Napi::Value NAVUtil::GetChannelDescription(const Napi::CallbackInfo& callback) {
#ifdef FFMPEG_5_1
    auto channel = (AVChannel) callback[0].As<Napi::Number>().Int32Value();
    char buf[128];
    memset(buf, 0, 128);
    int length = av_channel_description(buf, 127, channel);
    
    if (length < 0)
        nlav_throw(callback.Env(), length, "av_channel_description");
    
    return Napi::String::New(callback.Env(), buf);
#else
    auto channel = callback[0].As<Napi::Number>().Int32Value();
    return Napi::String::New(callback.Env(), av_get_channel_description(channel));
#endif
}

Napi::Value NAVUtil::GetChannelFromName(const Napi::CallbackInfo& callback) {
#ifdef FFMPEG_5_1
    return Napi::Number::New(callback.Env(), av_channel_from_string(callback[0].As<Napi::String>().Utf8Value().c_str()));
#else
    Napi::TypeError::New(callback.Env(), "Not available before ffmpeg 5.1")
        .ThrowAsJavaScriptException();
    return callback.Env().Undefined();
#endif
}

Napi::Value NAVUtil::GetChannelLayout(const Napi::CallbackInfo& callback) {
    return Napi::Number::New(callback.Env(), av_get_channel_layout(callback[0].As<Napi::String>().Utf8Value().c_str()));
}

Napi::Value NAVUtil::GetExtendedChannelLayout(const Napi::CallbackInfo& callback) {
    uint64_t channelLayout;
    int numberOfChannels;

    int result = Napi::Number::New(
        callback.Env(), 
        av_get_extended_channel_layout(
            callback[0].As<Napi::String>().Utf8Value().c_str(),
            &channelLayout,
            &numberOfChannels
        )
    );

    if (result < 0)
        return nlav_throw(callback.Env(), result, "av_get_extended_channel_layout");
    
    auto obj = Napi::Object::New(callback.Env());

    obj.Set("layout", Napi::Number::New(callback.Env(), channelLayout));
    obj.Set("numberOfChannels", Napi::Number::New(callback.Env(), numberOfChannels));

    return obj;
}

Napi::Value NAVUtil::GetChannelLayoutName(const Napi::CallbackInfo& callback) {
    auto layout = callback[0].As<Napi::Number>().Int32Value();
    auto channelCount = 0;
    
    if (callback.Length() > 1)
        channelCount = callback[1].As<Napi::Number>().Int32Value();
    
    char name[128];
    av_get_channel_layout_string(name, sizeof(name), channelCount, layout);

    return Napi::String::New(callback.Env(), name);
}

Napi::Value NAVUtil::GetNumberOfChannelsInLayout(const Napi::CallbackInfo& callback) {
    return Napi::Number::New(
        callback.Env(), 
        av_get_channel_layout_nb_channels(callback[0].As<Napi::Number>().Int32Value())
    );
}

Napi::Value NAVUtil::GetDefaultChannelLayout(const Napi::CallbackInfo& callback) {
    return Napi::Number::New(
        callback.Env(), 
        av_get_default_channel_layout(callback[0].As<Napi::Number>().Int32Value())
    );
}

Napi::Value NAVUtil::GetIndexOfChannelInLayout(const Napi::CallbackInfo& callback) {
    auto result = av_get_channel_layout_channel_index(
        (uint64_t)callback[0].As<Napi::Number>().Int64Value(),
        (uint64_t)callback[1].As<Napi::Number>().Int64Value()
    );

    if (result < 0)
        return nlav_throw(callback.Env(), result, "av_get_channel_layout_channel_index");

    return Napi::Number::New(callback.Env(), result);
}

Napi::Value NAVUtil::GetChannelInLayoutByIndex(const Napi::CallbackInfo& callback) {
    return Napi::Number::New(
        callback.Env(),
        av_channel_layout_extract_channel(
            (uint64_t)callback[0].As<Napi::Number>().Int64Value(),
            (uint64_t)callback[1].As<Napi::Number>().Int64Value()
        )
    );
}
