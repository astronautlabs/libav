#include "../common.h"

extern "C" {
    #include <libavutil/avutil.h>
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
