#include <napi.h>

#include "../common.h"
#include "../resource.h"

extern "C" {
    #include <libavutil/frame.h>
}

class NAVFrame : public NAVResource<NAVFrame, AVFrame> {
    public:
        NAVFrame(const Napi::CallbackInfo& info);
        
        inline static std::string ExportName() { return "AVFrame"; }
        inline static Napi::Function ClassDefinition(const Napi::Env &env) {
            return DefineClass(env, "AVFrame", {
                
                // Static Methods
                StaticMethod("getSideDataName", &NAVFrame::GetSideDataName),

                // Methods

                R_METHOD("referTo", &NAVFrame::ReferTo),
                R_METHOD("unrefer", &NAVFrame::Unrefer),
                R_METHOD("clone", &NAVFrame::Clone),
                R_METHOD("copyTo", &NAVFrame::CopyTo),
                R_METHOD("copyPropertiesTo", &NAVFrame::CopyPropertiesTo),
                R_METHOD("getPlaneBuffer", &NAVFrame::GetPlaneBuffer),
                R_METHOD("addSideData", &NAVFrame::AddSideData),
                R_METHOD("getSideData", &NAVFrame::GetSideData),
                R_METHOD("removeSideData", &NAVFrame::RemoveSideData),
                R_METHOD("applyCropping", &NAVFrame::ApplyCropping),
                R_METHOD("moveReferenceFrom", &NAVFrame::MoveReferenceFrom),
                R_METHOD("allocateBuffer", &NAVFrame::AllocateBuffer),

                R_ACCESSOR("writable", &NAVFrame::IsWritable, nullptr),
                R_ACCESSOR("lineSizes", &NAVFrame::GetLineSize, &NAVFrame::SetLineSize),
                R_ACCESSOR("width", &NAVFrame::GetWidth, &NAVFrame::SetWidth),
                R_ACCESSOR("height", &NAVFrame::GetHeight, &NAVFrame::SetHeight),
                R_ACCESSOR("numberOfSamples", &NAVFrame::GetNumberOfSamples, &NAVFrame::SetNumberOfSamples),
                R_ACCESSOR("format", &NAVFrame::GetFormat, &NAVFrame::SetFormat),
                R_ACCESSOR("keyFrame", &NAVFrame::GetIsKeyFrame, &NAVFrame::SetIsKeyFrame),
                R_ACCESSOR("pictureType", &NAVFrame::GetPictType, &NAVFrame::SetPictType),
                R_ACCESSOR("sampleAspectRatio", &NAVFrame::GetSampleAspectRatio, &NAVFrame::SetSampleAspectRatio),
                R_ACCESSOR("pts", &NAVFrame::GetPts, &NAVFrame::SetPts),
                R_ACCESSOR("packetDts", &NAVFrame::GetPacketDts, &NAVFrame::SetPacketDts),
                R_ACCESSOR("timeBase", &NAVFrame::GetTimeBase, &NAVFrame::SetTimeBase),
                R_ACCESSOR("codedPictureNumber", &NAVFrame::GetCodedPictureNumber, &NAVFrame::SetCodedPictureNumber),
                R_ACCESSOR("displayPictureNumber", &NAVFrame::GetDisplayPictureNumber, &NAVFrame::SetDisplayPictureNumber),
                R_ACCESSOR("quality", &NAVFrame::GetQuality, &NAVFrame::SetQuality),
                R_ACCESSOR("repeatPicture", &NAVFrame::GetRepeatPict, &NAVFrame::SetRepeatPict),
                R_ACCESSOR("interlaced", &NAVFrame::GetInterlacedFrame, &NAVFrame::SetInterlacedFrame),
                R_ACCESSOR("topFieldFirst", &NAVFrame::GetTopFieldFirst, &NAVFrame::SetTopFieldFirst),
                R_ACCESSOR("paletteHasChanged", &NAVFrame::GetHasPaletteChanged, &NAVFrame::SetHasPaletteChanged),
                R_ACCESSOR("reorderedOpaque", &NAVFrame::GetReorderedOpaque, &NAVFrame::SetReorderedOpaque),
                R_ACCESSOR("sampleRate", &NAVFrame::GetSampleRate, &NAVFrame::SetSampleRate),
                R_ACCESSOR("buffers", &NAVFrame::GetBuffers, nullptr),
                R_ACCESSOR("extendedBuffers", &NAVFrame::GetExtendedBuffers, nullptr),
                R_ACCESSOR("sideData", &NAVFrame::GetSideDatas, nullptr),
                R_ACCESSOR("flags", &NAVFrame::GetFlags, &NAVFrame::SetFlags),
                R_ACCESSOR("colorRange", &NAVFrame::GetColorRange, &NAVFrame::SetColorRange),
                R_ACCESSOR("colorPrimaries", &NAVFrame::GetColorPrimaries, &NAVFrame::SetColorPrimaries),
                R_ACCESSOR("colorTransferCharacteristics", &NAVFrame::GetColorTrc, &NAVFrame::SetColorTrc),
                R_ACCESSOR("colorSpace", &NAVFrame::GetColorSpace, &NAVFrame::SetColorSpace),
                R_ACCESSOR("chromaLocation", &NAVFrame::GetChromaLocation, &NAVFrame::SetChromaLocation),
                R_ACCESSOR("bestEffortTimestamp", &NAVFrame::GetBestEffortTimestamp, &NAVFrame::SetBestEffortTimestamp),
                R_ACCESSOR("packetPosition", &NAVFrame::GetPacketPos, &NAVFrame::SetPacketPos),
                R_ACCESSOR("packetDuration", &NAVFrame::GetPacketDuration, &NAVFrame::SetPacketDuration),
                R_ACCESSOR("metadata", &NAVFrame::GetMetadata, &NAVFrame::SetMetadata),
                R_ACCESSOR("decodeErrorFlags", &NAVFrame::GetDecodeErrorFlags, &NAVFrame::SetDecodeErrorFlags),
                R_ACCESSOR("packetSize", &NAVFrame::GetPacketSize, &NAVFrame::SetPacketSize),
                R_ACCESSOR("hwFramesContext", &NAVFrame::GetHwFramesContext, &NAVFrame::SetHwFramesContext),
                R_ACCESSOR("opaqueRef", &NAVFrame::GetOpaqueRef, &NAVFrame::SetOpaqueRef),
                R_ACCESSOR("cropTop", &NAVFrame::GetCropTop, &NAVFrame::SetCropTop),
                R_ACCESSOR("cropBottom", &NAVFrame::GetCropBottom, &NAVFrame::SetCropBottom),
                R_ACCESSOR("cropLeft", &NAVFrame::GetCropLeft, &NAVFrame::SetCropLeft),
                R_ACCESSOR("cropRight", &NAVFrame::GetCropRight, &NAVFrame::SetCropRight),
                R_ACCESSOR("channelLayout", &NAVFrame::GetChannelLayout, &NAVFrame::SetChannelLayout),
            });
        }

        virtual void Free();
        virtual void RefHandle();

    private:
        bool Owned = true;

        // Static methods

        static Napi::Value GetSideDataName(const Napi::CallbackInfo& info);

        // Methods

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
        Napi::Value MoveReferenceFrom(const Napi::CallbackInfo& info);
        Napi::Value AllocateBuffer(const Napi::CallbackInfo& info);
        Napi::Value IsWritable(const Napi::CallbackInfo& info);

        // Properties

        Napi::Value GetLineSize(const Napi::CallbackInfo& info);
        void SetLineSize(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetWidth(const Napi::CallbackInfo& info);
        void SetWidth(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetHeight(const Napi::CallbackInfo& info);
        void SetHeight(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetNumberOfSamples(const Napi::CallbackInfo& info);
        void SetNumberOfSamples(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetFormat(const Napi::CallbackInfo& info);
        void SetFormat(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetIsKeyFrame(const Napi::CallbackInfo& info);
        void SetIsKeyFrame(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetPictType(const Napi::CallbackInfo& info);
        void SetPictType(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetSampleAspectRatio(const Napi::CallbackInfo& info);
        void SetSampleAspectRatio(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetPts(const Napi::CallbackInfo& info);
        void SetPts(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetPacketDts(const Napi::CallbackInfo& info);
        void SetPacketDts(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetTimeBase(const Napi::CallbackInfo& info);
        void SetTimeBase(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetCodedPictureNumber(const Napi::CallbackInfo& info);
        void SetCodedPictureNumber(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetDisplayPictureNumber(const Napi::CallbackInfo& info);
        void SetDisplayPictureNumber(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetQuality(const Napi::CallbackInfo& info);
        void SetQuality(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetRepeatPict(const Napi::CallbackInfo& info);
        void SetRepeatPict(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetInterlacedFrame(const Napi::CallbackInfo& info);
        void SetInterlacedFrame(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetIsTopFieldFirst(const Napi::CallbackInfo& info);
        void SetIsTopFieldFirst(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetTopFieldFirst(const Napi::CallbackInfo& info);
        void SetTopFieldFirst(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetHasPaletteChanged(const Napi::CallbackInfo& info);
        void SetHasPaletteChanged(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetReorderedOpaque(const Napi::CallbackInfo& info);
        void SetReorderedOpaque(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetSampleRate(const Napi::CallbackInfo& info);
        void SetSampleRate(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetBuffers(const Napi::CallbackInfo& info);
        Napi::Value GetExtendedBuffers(const Napi::CallbackInfo& info);
        Napi::Value GetSideDatas(const Napi::CallbackInfo& info);
        Napi::Value GetFlags(const Napi::CallbackInfo& info);
        void SetFlags(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetColorRange(const Napi::CallbackInfo& info);
        void SetColorRange(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetColorPrimaries(const Napi::CallbackInfo& info);
        void SetColorPrimaries(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetColorTrc(const Napi::CallbackInfo& info);
        void SetColorTrc(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetColorSpace(const Napi::CallbackInfo& info);
        void SetColorSpace(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetChromaLocation(const Napi::CallbackInfo& info);
        void SetChromaLocation(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetBestEffortTimestamp(const Napi::CallbackInfo& info);
        void SetBestEffortTimestamp(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetPacketPos(const Napi::CallbackInfo& info);
        void SetPacketPos(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetPacketDuration(const Napi::CallbackInfo& info);
        void SetPacketDuration(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetMetadata(const Napi::CallbackInfo& info);
        void SetMetadata(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetDecodeErrorFlags(const Napi::CallbackInfo& info);
        void SetDecodeErrorFlags(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetPacketSize(const Napi::CallbackInfo& info);
        void SetPacketSize(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetHwFramesContext(const Napi::CallbackInfo& info);
        void SetHwFramesContext(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetOpaqueRef(const Napi::CallbackInfo& info);
        void SetOpaqueRef(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetCropTop(const Napi::CallbackInfo& info);
        void SetCropTop(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetCropBottom(const Napi::CallbackInfo& info);
        void SetCropBottom(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetCropLeft(const Napi::CallbackInfo& info);
        void SetCropLeft(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetCropRight(const Napi::CallbackInfo& info);
        void SetCropRight(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetChannelLayout(const Napi::CallbackInfo& info);
        void SetChannelLayout(const Napi::CallbackInfo& info, const Napi::Value& value);
};

class NAVFrameSideData : public NAVResource<NAVFrameSideData, AVFrameSideData> {
    public:
        NAVFrameSideData(const Napi::CallbackInfo& info);

        inline static std::string ExportName() { return "AVFrameSideData"; }
        inline static Napi::Function ClassDefinition(const Napi::Env &env) {
            return DefineClass(env, "AVFrameSideData", {
                R_GETTER("type", &NAVFrameSideData::GetType),
                R_GETTER("size", &NAVFrameSideData::GetSize),
                R_GETTER("metadata", &NAVFrameSideData::GetMetadata),
                R_GETTER("buffer", &NAVFrameSideData::GetBuffer)
            });
        }

        virtual void Free();

    private:
        Napi::Value GetType(const Napi::CallbackInfo& info);
        Napi::Value GetSize(const Napi::CallbackInfo& info);
        Napi::Value GetMetadata(const Napi::CallbackInfo& info);
        Napi::Value GetBuffer(const Napi::CallbackInfo& info);

};