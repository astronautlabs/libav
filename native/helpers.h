#include <napi.h>
extern "C" {
    #include <libavutil/rational.h>
}

static inline Napi::Value NRational(napi_env env, AVRational rational) {
    auto obj = Napi::Object::New(env);

    obj.Set("num", rational.num);
    obj.Set("den", rational.den);

    return obj;
}