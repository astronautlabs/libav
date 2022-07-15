#include "../common.h"
#include <napi.h>

Napi::Value nlavu_avutil_version(const Napi::CallbackInfo& callback);
Napi::Value nlavu_av_version_info(const Napi::CallbackInfo& callback);
Napi::Value nlavu_avutil_configuration(const Napi::CallbackInfo& callback);
Napi::Value nlavu_avutil_license(const Napi::CallbackInfo& callback);
Napi::Value nlavu_av_get_media_type_string(const Napi::CallbackInfo& callback);
Napi::Value nlavu_av_get_picture_type_char(const Napi::CallbackInfo& callback);
Napi::Value nlavu_av_int_list_length_for_size(const Napi::CallbackInfo& callback);
Napi::Value nlavu_av_fopen_utf8(const Napi::CallbackInfo& callback);
Napi::Value nlavu_av_get_time_base_q(const Napi::CallbackInfo& callback);
Napi::Value nlavu_av_fourcc_make_string(const Napi::CallbackInfo& callback);