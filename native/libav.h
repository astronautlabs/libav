
#ifndef __LIBAV_H
#define __LIBAV_H

#include <napi.h>

class LibAvAddon : public Napi::Addon<LibAvAddon>
{
public:
    LibAvAddon(Napi::Env env, Napi::Object exports);
    static LibAvAddon *Self(const Napi::Env &env);
    static LibAvAddon *Self(const Napi::CallbackInfo &call);

    Napi::FunctionReference *AVBuffer;
    Napi::FunctionReference *AVBufferPool;
};

#endif // __LIBAV_H