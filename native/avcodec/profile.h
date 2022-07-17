#include "../common.h"

#include <napi.h>
#include "../resource.h"

extern "C" {
    #include <libavcodec/avcodec.h>
}

class NAVProfile : public NAVResource<NAVProfile, AVProfile> {
    public:
        NAVProfile(const Napi::CallbackInfo& info);
        
        inline static std::string ExportName() { return "AVProfile"; }
        inline static Napi::Function ClassDefinition(const Napi::Env &env) {
            return DefineClass(env, "AVProfile", {
                R_GETTER("id", &NAVProfile::GetId),
                R_GETTER("name", &NAVProfile::GetName)
            });
        }

        virtual void Free();
    private:
        
        Napi::Value GetId(const Napi::CallbackInfo& info);
        Napi::Value GetName(const Napi::CallbackInfo& info);
};