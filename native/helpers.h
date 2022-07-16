#ifndef __HELPERS_H__
#   define __HELPERS_H__

#include <napi.h>
#include <assert.h>

extern "C" {
    #include <libavutil/rational.h>
}

static inline Napi::Value NRational(napi_env env, AVRational rational) {
    auto obj = Napi::Object::New(env);

    obj.Set("num", rational.num);
    obj.Set("den", rational.den);

    return obj;
}

template<class T>
static inline T *assert_valid(T *ptr) {
    assert(ptr);
    return ptr;
}

#endif // #ifndef __HELPERS_H__