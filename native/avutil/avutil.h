#include "../common.h"
#include <napi.h>
#include "../resource.h"

class NAVUtil : public NAVResource<NAVUtil, void> {
    public:
        NAVUtil(const Napi::CallbackInfo& info);
        
        inline static std::string ExportName() { return "AVUtil"; }
        inline static Napi::Function ClassDefinition(const Napi::Env &env) {
            return DefineClass(env, "AVUtil", {
                StaticAccessor("version", &NAVUtil::GetVersion, nullptr),
                StaticAccessor("versionInfo", &NAVUtil::GetVersionInfo, nullptr),
                StaticAccessor("configuration", &NAVUtil::GetConfiguration, nullptr),
                StaticAccessor("license", &NAVUtil::GetLicense, nullptr),
                StaticAccessor("timebase", &NAVUtil::GetTimeBase, nullptr),

                StaticMethod("getMediaTypeString", &NAVUtil::GetMediaTypeString),
                StaticMethod("getPictureTypeString", &NAVUtil::GetPictureTypeString),
                StaticMethod("getFourCCString", &NAVUtil::GetFourCCString),
                StaticMethod("getChannelName", &NAVUtil::GetChannelName),
                StaticMethod("getChannelDescription", &NAVUtil::GetChannelDescription),
                StaticMethod("getChannelFromName", &NAVUtil::GetChannelFromName),
            });
        }

        virtual void Free();
    private:
        // Static Properties

        static Napi::Value GetVersion(const Napi::CallbackInfo& callback);
        static Napi::Value GetVersionInfo(const Napi::CallbackInfo& callback);
        static Napi::Value GetConfiguration(const Napi::CallbackInfo& callback);
        static Napi::Value GetLicense(const Napi::CallbackInfo& callback);
        static Napi::Value GetTimeBase(const Napi::CallbackInfo& callback);

        // Static Methods

        static Napi::Value GetMediaTypeString(const Napi::CallbackInfo& callback);
        static Napi::Value GetPictureTypeString(const Napi::CallbackInfo& callback);
        static Napi::Value GetFourCCString(const Napi::CallbackInfo& callback);
        static Napi::Value GetChannelName(const Napi::CallbackInfo& callback);
        static Napi::Value GetChannelDescription(const Napi::CallbackInfo& callback);
        static Napi::Value GetChannelFromName(const Napi::CallbackInfo& callback);
};