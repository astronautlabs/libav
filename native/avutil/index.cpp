#include "index.h"
#include "avutil.h"
#include "buffer.h"
#include "dict.h"
#include "frame.h"
#include "class.h"

void nlavu_init(Napi::Env env, Napi::Object exports) {

    // avutil.ts

    exports.Set("nlavu_avutil_version", Napi::Function::New(env, nlavu_avutil_version));
    exports.Set("nlavu_av_version_info", Napi::Function::New(env, nlavu_av_version_info));
    exports.Set("nlavu_avutil_configuration", Napi::Function::New(env, nlavu_avutil_configuration));
    exports.Set("nlavu_avutil_license", Napi::Function::New(env, nlavu_avutil_license));
    exports.Set("nlavu_av_get_media_type_string", Napi::Function::New(env, nlavu_av_get_media_type_string));
    exports.Set("nlavu_av_get_picture_type_char", Napi::Function::New(env, nlavu_av_get_picture_type_char));
    exports.Set("nlavu_av_int_list_length_for_size", Napi::Function::New(env, nlavu_av_int_list_length_for_size));
    exports.Set("nlavu_av_fopen_utf8", Napi::Function::New(env, nlavu_av_fopen_utf8));
    exports.Set("nlavu_av_get_time_base_q", Napi::Function::New(env, nlavu_av_get_time_base_q));
    exports.Set("nlavu_av_fourcc_make_string", Napi::Function::New(env, nlavu_av_fourcc_make_string));

    // buffer.ts
    
    NAVBuffer::Register(env, exports);
    NAVBufferPool::Register(env, exports);
    NAVDictionary::Register(env, exports);
    NAVDictionaryEntry::Register(env, exports);
    NAVFrame::Register(env, exports);
    NAVClass::Register(env, exports);
    //NAVFrameSideData::Init(env, exports);
}