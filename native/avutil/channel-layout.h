#include "../common.h"

#include <napi.h>
#include "../resource.h"

extern "C" {
    #include <libavutil/channel_layout.h>
}

class NAVChannelLayout : public NAVResource<NAVChannelLayout, AVChannelLayout> {
    public:
        NAVChannelLayout(const Napi::CallbackInfo& info);
        
        inline static std::string ExportName() { return "AVChannelLayout"; }
        inline static Napi::Function ClassDefinition(const Napi::Env &env) {
            return DefineClass(env, "AVChannelLayout", {
                R_GETTER("order", &NAVChannelLayout::GetOrder),
                R_GETTER("count", &NAVChannelLayout::GetCount)
            });
        }

        virtual void Free();
    private:
        Napi::Value GetOrder(const Napi::CallbackInfo& info);
        Napi::Value GetCount(const Napi::CallbackInfo& info);
};