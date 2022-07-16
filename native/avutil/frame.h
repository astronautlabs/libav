#include "../common.h"

#include <napi.h>

extern "C" {
    #include <libavutil/frame.h>
}

class NAVFrame : public Napi::ObjectWrap<NAVFrame> {
    public:
        NAVFrame(const Napi::CallbackInfo& info);
        static void Init(Napi::Env env, Napi::Object exports);
        virtual void Finalize(Napi::Env env);
        AVFrame *handle;

    private:
        Napi::Value ReferTo(const Napi::CallbackInfo& info);
        Napi::Value Unrefer(const Napi::CallbackInfo& info);
        Napi::Value Clone(const Napi::CallbackInfo& info);
        Napi::Value CopyTo(const Napi::CallbackInfo& info);
        Napi::Value CopyPropertiesTo(const Napi::CallbackInfo& info);
        Napi::Value GetPlaneBuffer(const Napi::CallbackInfo& info);
        Napi::Value AddSideData(const Napi::CallbackInfo& info);
        Napi::Value GetSideData(const Napi::CallbackInfo& info);
        Napi::Value RemoveSideData(const Napi::CallbackInfo& info);
        Napi::Value ApplyCropping(const Napi::CallbackInfo& info);
        Napi::Value GetSideDataName(const Napi::CallbackInfo& info);

        Napi::Value MoveReference(const Napi::CallbackInfo& info);
        Napi::Value GetBuffer(const Napi::CallbackInfo& info);
        
        Napi::Value IsWritable(const Napi::CallbackInfo& info);
        Napi::Value MakeWritable(const Napi::CallbackInfo& info);

        Napi::Value GetData(const Napi::CallbackInfo& info);
        Napi::Value GetLineSize(const Napi::CallbackInfo& info);
        Napi::Value GetExtendedData(const Napi::CallbackInfo& info);
        Napi::Value GetWidth(const Napi::CallbackInfo& info);
        Napi::Value GetHeight(const Napi::CallbackInfo& info);
        Napi::Value GetNumberOfSamples(const Napi::CallbackInfo& info);
        Napi::Value GetFormat(const Napi::CallbackInfo& info);
        Napi::Value IsKeyFrame(const Napi::CallbackInfo& info);
        Napi::Value GetPictType(const Napi::CallbackInfo& info);
        Napi::Value GetSampleAspectRatio(const Napi::CallbackInfo& info);
        Napi::Value GetPts(const Napi::CallbackInfo& info);
        Napi::Value GetPacketPts(const Napi::CallbackInfo& info);
        Napi::Value GetTimeBase(const Napi::CallbackInfo& info);
        Napi::Value GetCodedPictureNumber(const Napi::CallbackInfo& info);
        Napi::Value GetDisplayPictureNumber(const Napi::CallbackInfo& info);
        Napi::Value GetQuality(const Napi::CallbackInfo& info);
        Napi::Value GetRepeatPict(const Napi::CallbackInfo& info);
        Napi::Value GetInterlacedFrame(const Napi::CallbackInfo& info);
        Napi::Value IsInterlacedFrame(const Napi::CallbackInfo& info);
        Napi::Value IsTopFieldFirst(const Napi::CallbackInfo& info);
        Napi::Value GetTopFieldFirst(const Napi::CallbackInfo& info);
        Napi::Value HasPaletteChanged(const Napi::CallbackInfo& info);
        Napi::Value GetReorderedOpaque(const Napi::CallbackInfo& info);
        Napi::Value GetSampleRate(const Napi::CallbackInfo& info);
        Napi::Value GetBuffers(const Napi::CallbackInfo& info);
        Napi::Value GetExtendedBuffers(const Napi::CallbackInfo& info);
        Napi::Value GetSideDatas(const Napi::CallbackInfo& info);
        Napi::Value GetFlags(const Napi::CallbackInfo& info);
        Napi::Value GetColorRange(const Napi::CallbackInfo& info);
        Napi::Value GetColorPrimaries(const Napi::CallbackInfo& info);
        Napi::Value GetColorTrc(const Napi::CallbackInfo& info);
        Napi::Value GetColorSpace(const Napi::CallbackInfo& info);
        Napi::Value GetChromaLocation(const Napi::CallbackInfo& info);
        Napi::Value GetBestEffortTimestamp(const Napi::CallbackInfo& info);
        Napi::Value GetPacketPos(const Napi::CallbackInfo& info);
        Napi::Value GetPacketDuration(const Napi::CallbackInfo& info);
        Napi::Value GetMetadata(const Napi::CallbackInfo& info);
        Napi::Value GetDecodeErrorFlags(const Napi::CallbackInfo& info);
        Napi::Value GetPacketSize(const Napi::CallbackInfo& info);
        Napi::Value GetHwFramesContext(const Napi::CallbackInfo& info);
        Napi::Value GetOpaqueRef(const Napi::CallbackInfo& info);
        Napi::Value GetCropTop(const Napi::CallbackInfo& info);
        Napi::Value GetCropBottom(const Napi::CallbackInfo& info);
        Napi::Value GetCropLeft(const Napi::CallbackInfo& info);
        Napi::Value GetCropRight(const Napi::CallbackInfo& info);
        Napi::Value GetChannelLayout(const Napi::CallbackInfo& info);
};

// class NAVFrameSideData : public Napi::ObjectWrap<NAVFrameSideData> {
//     public:
//         NAVFrameSideData(const Napi::CallbackInfo& info);
//         static void Init(Napi::Env env, Napi::Object exports);
//         virtual void Finalize(Napi::Env env);
//         AVFrameSideData *handle;

//         static Napi::Value FromHandle(const Napi::Env &env, AVFrameSideData *handle);

//     private:
//         Napi::Value GetType(const Napi::CallbackInfo& info);
//         Napi::Value GetData(const Napi::CallbackInfo& info);
//         Napi::Value GetSize(const Napi::CallbackInfo& info);
//         Napi::Value GetMetadata(const Napi::CallbackInfo& info);
//         Napi::Value GetBuffer(const Napi::CallbackInfo& info);

// };