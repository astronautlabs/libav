#include "libavaddon.h"
#include "avutil/index.h"
#include "avcodec/index.h"

LibAvAddon::LibAvAddon(Napi::Env env, Napi::Object exports) {

    // This will be done later for us, but we want to access it during initialization phase.
    env.SetInstanceData<LibAvAddon>(this);

    // Modules
    nlavu_init(env, exports);
    nlavc_init(env, exports);

    DefineAddon(exports, {});
}

LibAvAddon *LibAvAddon::Self(const Napi::Env &env) {
    return env.GetInstanceData<LibAvAddon>();
}

LibAvAddon *LibAvAddon::Self(const Napi::CallbackInfo &call) {
    return Self(call.Env());
}

NODE_API_ADDON(LibAvAddon)