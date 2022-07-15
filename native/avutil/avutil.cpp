#include "../common.h"

extern "C" {
    #include <libavutil/avutil.h>
}

#include "avutil.h"
#include "../helpers.h"

Napi::Value nlavu_avutil_version(const Napi::CallbackInfo& callback) {
    return Napi::Number::New(callback.Env(), avutil_version());
}

Napi::Value nlavu_av_version_info(const Napi::CallbackInfo& callback) {
    return Napi::String::New(callback.Env(), av_version_info());
}

Napi::Value nlavu_avutil_configuration(const Napi::CallbackInfo& callback) {
    return Napi::String::New(callback.Env(), avutil_configuration());
}

Napi::Value nlavu_avutil_license(const Napi::CallbackInfo& callback) {
    return Napi::String::New(callback.Env(), avutil_license());
}

Napi::Value nlavu_av_get_media_type_string(const Napi::CallbackInfo& callback) {
    auto media_type = callback[0].As<Napi::Number>().Int32Value();
    return Napi::String::New(callback.Env(), av_get_media_type_string((AVMediaType)media_type));
}

Napi::Value nlavu_av_get_picture_type_char(const Napi::CallbackInfo& callback) {
    auto pict_type = callback[0].As<Napi::Number>().Int32Value();
    char buf[2];

    buf[1] = 0;
    buf[0] = av_get_picture_type_char((AVPictureType) pict_type);
    return Napi::String::New(callback.Env(), (const char *)&buf);
}

Napi::Value nlavu_av_int_list_length_for_size(const Napi::CallbackInfo& callback) {
    auto elsize = callback[0].As<Napi::Number>().Int32Value();
    auto list = callback[1].As<Napi::External<void>>().Data();
    auto term = callback[0].As<Napi::Number>().Int64Value();

    return Napi::Number::New(callback.Env(), av_int_list_length_for_size(elsize, list, term));
}

Napi::Value nlavu_av_fopen_utf8(const Napi::CallbackInfo& callback) {
    auto path = callback[0].As<Napi::String>().Utf8Value();
    auto mode = callback[1].As<Napi::String>().Utf8Value();
    return Napi::External<FILE>::New(callback.Env(), av_fopen_utf8(path.c_str(), mode.c_str()));
}

Napi::Value nlavu_av_get_time_base_q(const Napi::CallbackInfo& callback) {
    return NRational(callback.Env(), av_get_time_base_q());

}

Napi::Value nlavu_av_fourcc_make_string(const Napi::CallbackInfo& callback) {
    auto fourcc = callback[0].As<Napi::Number>().Uint32Value();
    char buf[AV_FOURCC_MAX_STRING_SIZE];

    av_fourcc_make_string((char *)&buf, fourcc);

    return Napi::String::New(callback.Env(), (char *)buf);
}
