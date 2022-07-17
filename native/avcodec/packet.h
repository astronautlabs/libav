#include "../common.h"

#include <napi.h>
#include "../resource.h"

extern "C" {
    #include <libavcodec/packet.h>
}

class NAVPacket : public NAVResource<NAVPacket, AVPacket> {
    public:
        NAVPacket(const Napi::CallbackInfo& info);
        
        inline static std::string ExportName() { return "AVPacket"; }
        inline static Napi::Function ClassDefinition(const Napi::Env &env) {
            return DefineClass(env, "AVPacket", {
                R_GETTER("buffer", &NAVPacket::GetBuffer),
                R_GETTER("pts", &NAVPacket::GetPts),
                R_GETTER("dts", &NAVPacket::GetDts),
                R_GETTER("size", &NAVPacket::GetSize),
                R_GETTER("streamIndex", &NAVPacket::GetStreamIndex),
                R_GETTER("flags", &NAVPacket::GetFlags),
                R_GETTER("sideData", &NAVPacket::GetSideData),
                R_GETTER("sideDataCount", &NAVPacket::GetSideDataCount),
                R_GETTER("duration", &NAVPacket::Duration),
                R_GETTER("position", &NAVPacket::GetPosition),
                R_ACCESSOR("opaqueBuffer", &NAVPacket::GetOpaqueBuffer, &NAVPacket::SetOpaqueBuffer),
                R_GETTER("timeBase", &NAVPacket::GetTimeBase),
            });
        }

        virtual void Free();
        virtual void RefHandle();
    private:
        Napi::Value GetBuffer(const Napi::CallbackInfo& info);
        Napi::Value GetPts(const Napi::CallbackInfo& info);
        Napi::Value GetDts(const Napi::CallbackInfo& info);
        Napi::Value GetSize(const Napi::CallbackInfo& info);
        Napi::Value GetStreamIndex(const Napi::CallbackInfo& info);
        Napi::Value GetFlags(const Napi::CallbackInfo& info);
        Napi::Value GetSideData(const Napi::CallbackInfo& info);
        Napi::Value GetSideDataCount(const Napi::CallbackInfo& info);
        Napi::Value Duration(const Napi::CallbackInfo& info);
        Napi::Value GetPosition(const Napi::CallbackInfo& info);
        Napi::Value GetOpaqueBuffer(const Napi::CallbackInfo& info);
        void SetOpaqueBuffer(const Napi::CallbackInfo& info, const Napi::Value &value);
        Napi::Value GetTimeBase(const Napi::CallbackInfo& info);


};

class NAVPacketSideData : public NAVResource<NAVPacketSideData, AVPacketSideData> {
    public:
        NAVPacketSideData(const Napi::CallbackInfo& info);

        inline static std::string ExportName() { return "AVPacketSideData"; }
        inline static Napi::Function ClassDefinition(const Napi::Env &env) {
            return DefineClass(env, "AVPacketSideData", {
                R_GETTER("data", &NAVPacketSideData::GetData),
                R_GETTER("size", &NAVPacketSideData::GetSize),
                R_GETTER("type", &NAVPacketSideData::GetType)
            });
        }

        void Free();
    private:
        Napi::Reference<Napi::ArrayBuffer> ownedArrayBuffer;

        Napi::Value GetData(const Napi::CallbackInfo& info);
        Napi::Value GetSize(const Napi::CallbackInfo& info);
        Napi::Value GetType(const Napi::CallbackInfo& info);
};