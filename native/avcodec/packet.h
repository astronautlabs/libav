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

                // Methods
                
                R_METHOD("addSideData", &NAVPacket::AddSideData),
                R_METHOD("getSideData", &NAVPacket::GetSideData),

                // Properties 

                R_GETTER("buffer", &NAVPacket::GetBuffer),
                R_ACCESSOR("pts", &NAVPacket::GetPts, &NAVPacket::SetPts),
                R_ACCESSOR("dts", &NAVPacket::GetDts, &NAVPacket::SetDts),
                R_GETTER("size", &NAVPacket::GetSize),
                R_ACCESSOR("streamIndex", &NAVPacket::GetStreamIndex, &NAVPacket::SetStreamIndex),
                R_ACCESSOR("flags", &NAVPacket::GetFlags, &NAVPacket::SetFlags),
                R_GETTER("sideData", &NAVPacket::GetSideDatas),
                R_ACCESSOR("duration", &NAVPacket::GetDuration, &NAVPacket::SetDuration),
                R_ACCESSOR("position", &NAVPacket::GetPosition, &NAVPacket::SetPosition),
                R_ACCESSOR("opaqueBuffer", &NAVPacket::GetOpaqueBuffer, &NAVPacket::SetOpaqueBuffer),
                R_ACCESSOR("timeBase", &NAVPacket::GetTimeBase, &NAVPacket::SetTimeBase)
            });
        }

        virtual void Free();
        virtual void RefHandle();
    private:

        // Methods

        Napi::Value AddSideData(const Napi::CallbackInfo& info);
        Napi::Value GetSideData(const Napi::CallbackInfo& info);
        Napi::Value RemoveSideData(const Napi::CallbackInfo& info);

        // Properties

        Napi::Value GetBuffer(const Napi::CallbackInfo& info);
        Napi::Value GetPts(const Napi::CallbackInfo& info);
        void SetPts(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetDts(const Napi::CallbackInfo& info);
        void SetDts(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetSize(const Napi::CallbackInfo& info);
        Napi::Value GetStreamIndex(const Napi::CallbackInfo& info);
        void SetStreamIndex(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetFlags(const Napi::CallbackInfo& info);
        void SetFlags(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetSideDatas(const Napi::CallbackInfo& info);
        Napi::Value GetDuration(const Napi::CallbackInfo& info);
        void SetDuration(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetPosition(const Napi::CallbackInfo& info);
        void SetPosition(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetOpaqueBuffer(const Napi::CallbackInfo& info);
        void SetOpaqueBuffer(const Napi::CallbackInfo& info, const Napi::Value &value);
        Napi::Value GetTimeBase(const Napi::CallbackInfo& info);
        void SetTimeBase(const Napi::CallbackInfo& info, const Napi::Value& value);


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