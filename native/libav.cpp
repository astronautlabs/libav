#include "libav.h"
#include "avutil/index.h"

LibAvAddon::LibAvAddon(Napi::Env env, Napi::Object exports) {
    nlavu_init(env, exports);

    // In the constructor we declare the functions the add-on makes available
    // to JavaScript.
    DefineAddon(exports, {
        // Global instance methods / properties / etc can go here
    });
}

LibAvAddon *LibAvAddon::Self(const Napi::Env &env) {
    return env.GetInstanceData<LibAvAddon>();
}

LibAvAddon *LibAvAddon::Self(const Napi::CallbackInfo &call) {
    return Self(call.Env());
}

NODE_API_ADDON(LibAvAddon)