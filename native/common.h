#ifndef __NLAV_COMMON_H__
#  define __NLAV_COMMON_H__

#include <napi.h>

extern "C" {
    #include <libavutil/avutil.h>
}

inline std::string nlavu_get_error_string(int code) {
    char errbuf[64];
    return std::string(av_make_error_string(errbuf, 64, code));
}

inline Napi::Value nlav_throw(const Napi::Env &env, int code, std::string context = "") {
    std::string message = "libav: " + nlavu_get_error_string(code);
    if (context != "") {
        message = "[" + context + "] " + message;
    }

    Napi::Error::New(env, message)
        .ThrowAsJavaScriptException();
    return env.Undefined();
}

#endif // __NLAV_COMMON_H__