#include "../common.h"

#include <napi.h>
#include "../resource.h"

extern "C" {
    #include <libavutil/log.h>
}

class NAVClass : public NAVResource<NAVClass, AVClass> {
    public:
        NAVClass(const Napi::CallbackInfo& info);
        
        inline static std::string ExportName() { return "AVClass"; }
        inline static Napi::Function ClassDefinition(const Napi::Env &env) {
            return DefineClass(env, "AVClass", {
                R_GETTER("name", &NAVClass::GetName),
                R_GETTER("version", &NAVClass::GetVersion),
                R_GETTER("category", &NAVClass::GetCategory)
            });
        }

        virtual void Free();
    private:
        Napi::Value GetName(const Napi::CallbackInfo& info);
        Napi::Value GetVersion(const Napi::CallbackInfo& info);
        Napi::Value GetCategory(const Napi::CallbackInfo& info);

        Napi::Value GetContextName(const Napi::CallbackInfo& info);
        Napi::Value GetContextCategory(const Napi::CallbackInfo& info);
};