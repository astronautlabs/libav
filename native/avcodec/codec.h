#include "../common.h"

#include <napi.h>
#include "../resource.h"

extern "C" {
    #include <libavcodec/avcodec.h>
}

class NAVCodec : public NAVResource<NAVCodec, AVCodec> {
    public:
        NAVCodec(const Napi::CallbackInfo& info);
        
        inline static std::string ExportName() { return "AVCodec"; }
        inline static Napi::Function ClassDefinition(const Napi::Env &env) {
            return DefineClass(env, "AVCodec", {
                StaticAccessor("version", &NAVCodec::GetVersion, nullptr),
                StaticAccessor("configuration", &NAVCodec::GetConfiguration, nullptr),
                StaticAccessor("license", &NAVCodec::GetLicense, nullptr),
                
                StaticMethod("all", &NAVCodec::All),
                StaticMethod("findEncoder", &NAVCodec::FindEncoder),
                StaticMethod("findDecoder", &NAVCodec::FindDecoder),

                R_GETTER("name", &NAVCodec::GetName),
                R_GETTER("longName", &NAVCodec::GetLongName),
                R_GETTER("type", &NAVCodec::GetType),
                R_GETTER("id", &NAVCodec::GetId),
                R_GETTER("capabilities", &NAVCodec::GetCapabilities),
                R_GETTER("decoderMaxLowResolution", &NAVCodec::GetDecoderMaxLowResolution),
                R_GETTER("supportedFrameRates", &NAVCodec::GetSupportedFrameRates),
                R_GETTER("pixelFormats", &NAVCodec::GetPixelFormats),
                R_GETTER("supportedSampleRates", &NAVCodec::GetSupportedSampleRates),
                R_GETTER("sampleFormats", &NAVCodec::GetSampleFormats),
                R_GETTER("privateClass", &NAVCodec::GetPrivateClass),
                R_GETTER("profiles", &NAVCodec::GetProfiles),
                R_GETTER("wrapperName", &NAVCodec::GetWrapperName),
                R_GETTER("channelLayouts", &NAVCodec::GetChannelLayouts),
                R_GETTER("isEncoder", &NAVCodec::GetIsEncoder),
                R_GETTER("isDecoder", &NAVCodec::GetIsDecoder),
            });
        }

        virtual void Free();
    private:
        static Napi::Value GetVersion(const Napi::CallbackInfo& info);
        static Napi::Value GetConfiguration(const Napi::CallbackInfo& info);
        static Napi::Value GetLicense(const Napi::CallbackInfo& info);

        static Napi::Value All(const Napi::CallbackInfo& info);
        static Napi::Value FindDecoder(const Napi::CallbackInfo& info);
        static Napi::Value FindEncoder(const Napi::CallbackInfo& info);
        
        Napi::Value GetName(const Napi::CallbackInfo& info);
        Napi::Value GetLongName(const Napi::CallbackInfo& info);
        Napi::Value GetType(const Napi::CallbackInfo& info);
        Napi::Value GetId(const Napi::CallbackInfo& info);
        Napi::Value GetCapabilities(const Napi::CallbackInfo& info);
        Napi::Value GetDecoderMaxLowResolution(const Napi::CallbackInfo& info);
        Napi::Value GetSupportedFrameRates(const Napi::CallbackInfo& info);
        Napi::Value GetPixelFormats(const Napi::CallbackInfo& info);
        Napi::Value GetSupportedSampleRates(const Napi::CallbackInfo& info);
        Napi::Value GetSampleFormats(const Napi::CallbackInfo& info);
        Napi::Value GetPrivateClass(const Napi::CallbackInfo& info);
        Napi::Value GetProfiles(const Napi::CallbackInfo& info);
        Napi::Value GetWrapperName(const Napi::CallbackInfo& info);
        Napi::Value GetChannelLayouts(const Napi::CallbackInfo& info);
  
        Napi::Value GetIsEncoder(const Napi::CallbackInfo& info);
        Napi::Value GetIsDecoder(const Napi::CallbackInfo& info);
        Napi::Value GetProfileName(const Napi::CallbackInfo& info);
};