#include "../common.h"

#include <napi.h>
#include "../resource.h"

extern "C" {
    #include <libavutil/channel_layout.h>
}

#ifdef FFMPEG_5_1
class NAVChannelLayout : public NAVResource<NAVChannelLayout, AVChannelLayout> {
    public:
        NAVChannelLayout(const Napi::CallbackInfo& info);
        
        inline static std::string ExportName() { return "AVChannelLayout"; }
        inline static Napi::Function ClassDefinition(const Napi::Env &env) {
            return DefineClass(env, "AVChannelLayout", {
                // Static Methods

                StaticMethod("fromMask", &NAVChannelLayout::FromMask),
                StaticMethod("fromString", &NAVChannelLayout::FromString),
                StaticMethod("default", &NAVChannelLayout::FromDefault),

                // Static Properties

                StaticAccessor("standard", &NAVChannelLayout::GetStandards, nullptr),

                // Methods

                R_METHOD("uninit", &NAVChannelLayout::Uninit),
                
                // Properties

                R_ACCESSOR("order", &NAVChannelLayout::GetOrder, &NAVChannelLayout::SetOrder),
                R_ACCESSOR("count", &NAVChannelLayout::GetCount, &NAVChannelLayout::SetCount),
                R_ACCESSOR("mask", &NAVChannelLayout::GetMask, &NAVChannelLayout::SetMask),
                R_ACCESSOR("customOrder", &NAVChannelLayout::GetCustomOrder, &NAVChannelLayout::SetCustomOrder)
            });
        }

        virtual void Free();
        virtual void RefBuffer();
    private:
        bool owned = true;

        // Static Methods

        static Napi::Value FromMask(const Napi::CallbackInfo& info);
        static Napi::Value FromString(const Napi::CallbackInfo& info);
        static Napi::Value FromDefault(const Napi::CallbackInfo& info);

        // Static Properties
        static Napi::Value GetStandards(const Napi::CallbackInfo& info);

        // Methods

        Napi::Value Uninit(const Napi::CallbackInfo& info);

        // Properties

        Napi::Value GetOrder(const Napi::CallbackInfo& info);
        void SetOrder(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetCount(const Napi::CallbackInfo& info);
        void SetCount(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetMask(const Napi::CallbackInfo& info);
        void SetMask(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetCustomOrder(const Napi::CallbackInfo& info);
        void SetCustomOrder(const Napi::CallbackInfo& info, const Napi::Value& value);
};
#endif