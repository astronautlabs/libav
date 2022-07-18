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
static inline AVRational FromNRational(napi_env env, Napi::Value value) {
    auto obj = value.As<Napi::Object>();
    AVRational rational;

    rational.num = obj.Get("num").As<Napi::Number>().Int32Value();
    rational.den = obj.Get("den").As<Napi::Number>().Int32Value();

    return rational;
}

static inline Napi::Array NRationalArray(napi_env env, const AVRational *rational, int count) {
    Napi::Array array = Napi::Array::New(env, count);
    for (int i = 0, max = count; i < max; ++i)
        array.Set(i, NRational(env, *(rational + i)));
    return array;
}

template <typename FromT, typename ToT>
static inline std::vector<ToT> Transform(const std::vector<FromT> &items, std::function<ToT (FromT)> func) {
    std::vector<ToT> vec;
    for (FromT item : items)
        vec.push_back(func(item));

    return vec;
}

static inline Napi::Array VectorToArray(Napi::Env env, std::vector<Napi::Value> vec) {
    Napi::Array array = Napi::Array::New(env, vec.size());
    for (int i = 0, max = vec.size(); i < max; ++i)
        array.Set(i, vec[i]);
    return array;
}

static inline Napi::Array NRationalArrayZeroTerminated(napi_env env, const AVRational *rational) {
    std::vector<const AVRational*> vec;
    while (rational->den != 0 || rational->num != 0) {
        vec.push_back(rational);
        rational += 1;
    }

    return VectorToArray(
        env,
        Transform<const AVRational*, Napi::Value>(
            vec, 
            [=](const AVRational *value) { return NRational(env, *value); }
        )
    );
}

template <typename T>
static inline std::vector<T> TerminatedArray(const T *items, T terminationValue) {
    std::vector<T> vec;
    while (*items != terminationValue) {
        vec.push_back(*items);
        items += 1;
    }

    return vec;
}

template<class T>
static inline T *assert_valid(T *ptr) {
    assert(ptr);
    return ptr;
}

template <typename T>
std::function<Napi::Number(T)> WrapNumbers(Napi::Env env) {
    return [&](T t) { return Napi::Number::New(env, t); };
}

#endif // #ifndef __HELPERS_H__