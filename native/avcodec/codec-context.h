#include "../common.h"

#include <napi.h>
#include "../resource.h"
#include <thread>
#include <queue>
#include <condition_variable>
#include <mutex>

extern "C" {
    #include <libavcodec/avcodec.h>
}

struct WorkItem {
    AVPacket *packet;
    AVFrame *frame;
};

class NAVCodecContext : public NAVResource<NAVCodecContext, AVCodecContext> {
    public:
        NAVCodecContext(const Napi::CallbackInfo& info);
        
        inline static std::string ExportName() { return "AVCodecContext"; }
        inline static Napi::Function ClassDefinition(const Napi::Env &env) {
            return DefineClass(env, "AVCodecContext", {
                R_ACCESSOR("onFrame", &NAVCodecContext::GetOnFrame, &NAVCodecContext::SetOnFrame),
                R_ACCESSOR("onPacket", &NAVCodecContext::GetOnPacket, &NAVCodecContext::SetOnPacket),
                R_ACCESSOR("onError", &NAVCodecContext::GetOnError, &NAVCodecContext::SetOnError),

                R_METHOD("open", &NAVCodecContext::Open),
                R_METHOD("sendPacket", &NAVCodecContext::SendPacket),
                R_METHOD("sendFrame", &NAVCodecContext::SendFrame),

                R_GETTER("class", &NAVCodecContext::GetClass),
                R_GETTER("codecType", &NAVCodecContext::GetCodecType),
                R_GETTER("codecID", &NAVCodecContext::GetCodecId),
                R_GETTER("codecTag", &NAVCodecContext::GetCodecTag),
                R_ACCESSOR("bitRate", &NAVCodecContext::GetBitRate, &NAVCodecContext::SetBitRate),
                R_ACCESSOR("bitRateTolerance", &NAVCodecContext::GetBitRateTolerance, &NAVCodecContext::SetBitRateTolerance),
                R_ACCESSOR("globalQuality", &NAVCodecContext::GetGlobalQuality, &NAVCodecContext::SetGlobalQuality),
                R_ACCESSOR("compressionLevel", &NAVCodecContext::GetCompressionLevel, &NAVCodecContext::SetCompressionLevel),
                R_ACCESSOR("clags", &NAVCodecContext::GetFlags, &NAVCodecContext::SetFlags),
                R_ACCESSOR("clags2", &NAVCodecContext::GetFlags2, &NAVCodecContext::SetFlags2),
                R_ACCESSOR("timeBase", &NAVCodecContext::GetTimeBase, &NAVCodecContext::SetTimeBase),
                R_ACCESSOR("ticksPerFrame", &NAVCodecContext::GetTicksPerFrame, &NAVCodecContext::SetTicksPerFrame),
                R_ACCESSOR("delay", &NAVCodecContext::GetDelay, &NAVCodecContext::SetDelay),
                R_ACCESSOR("width", &NAVCodecContext::GetWidth, &NAVCodecContext::SetWidth),
                R_ACCESSOR("height", &NAVCodecContext::GetHeight, &NAVCodecContext::SetHeight),
                R_ACCESSOR("codedWidth", &NAVCodecContext::GetCodedWidth, &NAVCodecContext::SetCodedWidth),
                R_ACCESSOR("codedHeight", &NAVCodecContext::GetCodedHeight, &NAVCodecContext::SetCodedHeight),
                R_ACCESSOR("gopSize", &NAVCodecContext::GetGopSize, &NAVCodecContext::SetGopSize),
                R_ACCESSOR("pixelFormat", &NAVCodecContext::GetPixelFormat, &NAVCodecContext::SetPixelFormat),
                R_ACCESSOR("maxBFrames", &NAVCodecContext::GetMaxBFrames, &NAVCodecContext::SetMaxBFrames),
                R_ACCESSOR("bQuantizationFactor", &NAVCodecContext::GetBQuantizationFactor, &NAVCodecContext::SetBQuantizationFactor),
                R_ACCESSOR("bQuantizationOffset", &NAVCodecContext::GetBQuantizationOffset, &NAVCodecContext::SetBQuantizationOffset),
                R_ACCESSOR("hasBFrames", &NAVCodecContext::GetHasBFrames, &NAVCodecContext::SetHasBFrames),
                R_ACCESSOR("iQuantizationFactor", &NAVCodecContext::GetIQuantizationFactor, &NAVCodecContext::SetIQuantizationFactor),
                R_ACCESSOR("iQuantizationOffset", &NAVCodecContext::GetIQuantizationOffset, &NAVCodecContext::SetIQuantizationOffset),
                R_ACCESSOR("luminanceMasking", &NAVCodecContext::GetLuminanceMasking, &NAVCodecContext::SetLuminanceMasking),
                R_ACCESSOR("temporalComplexityMasking", &NAVCodecContext::GetTemporalComplexityMasking, &NAVCodecContext::SetTemporalComplexityMasking),
                R_ACCESSOR("spatialComplexityMasking", &NAVCodecContext::GetSpatialComplexityMasking, &NAVCodecContext::SetSpatialComplexityMasking),
                R_ACCESSOR("pMasking", &NAVCodecContext::GetPMasking, &NAVCodecContext::SetPMasking),
                R_ACCESSOR("darkMasking", &NAVCodecContext::GetDarkMasking, &NAVCodecContext::SetDarkMasking),
                R_ACCESSOR("sampleAspectRatio", &NAVCodecContext::GetSampleAspectRatio, &NAVCodecContext::SetSampleAspectRatio),
                R_ACCESSOR("motionEstimationComparisonFunction", &NAVCodecContext::GetMotionEstimationComparisonFunction, &NAVCodecContext::SetMotionEstimationComparisonFunction),
                R_ACCESSOR("subpixelMotionEstimationComparisonFunction", &NAVCodecContext::GetSubpixelMotionEstimationComparisonFunction, &NAVCodecContext::SetSubpixelMotionEstimationComparisonFunction),
                R_ACCESSOR("macroblockComparisonFunction", &NAVCodecContext::GetMacroblockComparisonFunction, &NAVCodecContext::SetMacroblockComparisonFunction),
                R_ACCESSOR("interlacedDCTComparisonFunction", &NAVCodecContext::GetInterlacedDCTComparisonFunction, &NAVCodecContext::SetInterlacedDCTComparisonFunction),
                R_ACCESSOR("diamondSize", &NAVCodecContext::GetDiamondSize, &NAVCodecContext::SetDiamondSize),
                R_ACCESSOR("lastPredictorCount", &NAVCodecContext::GetLastPredictorCount, &NAVCodecContext::SetLastPredictorCount),
                R_ACCESSOR("motionEstimationPrepassComparisonFunction", &NAVCodecContext::GetMotionEstimationPrepassComparisonFunction, &NAVCodecContext::SetMotionEstimationPrepassComparisonFunction),
                R_ACCESSOR("motionEstimationPrepassDiamondSize", &NAVCodecContext::GetMotionEstimationPrepassDiamondSize, &NAVCodecContext::SetMotionEstimationPrepassDiamondSize),
                R_ACCESSOR("motionEstimationSubpelQuality", &NAVCodecContext::GetMotionEstimationSubpelQuality, &NAVCodecContext::SetMotionEstimationSubpelQuality),
                R_ACCESSOR("motionEstimationRange", &NAVCodecContext::GetMotionEstimationRange, &NAVCodecContext::SetMotionEstimationRange),
                R_ACCESSOR("sliceFlags", &NAVCodecContext::GetSliceFlags, &NAVCodecContext::SetSliceFlags),
                R_ACCESSOR("macroblockDecisionMode", &NAVCodecContext::GetMacroblockDecisionMode, &NAVCodecContext::SetMacroblockDecisionMode),
                R_ACCESSOR("intraDCPrecision", &NAVCodecContext::GetIntraDCPrecision, &NAVCodecContext::SetIntraDCPrecision),
                R_ACCESSOR("skipTop", &NAVCodecContext::GetSkipTop, &NAVCodecContext::SetSkipTop),
                R_ACCESSOR("skipBottom", &NAVCodecContext::GetSkipBottom, &NAVCodecContext::SetSkipBottom),
                R_ACCESSOR("minMBLagrangeMultiplier", &NAVCodecContext::GetMinMBLagrangeMultiplier, &NAVCodecContext::SetMinMBLagrangeMultiplier),
                R_ACCESSOR("maxLBLagrangeMultiplier", &NAVCodecContext::GetMaxLBLagrangeMultiplier, &NAVCodecContext::SetMaxLBLagrangeMultiplier),
                R_ACCESSOR("bidirectionalRefine", &NAVCodecContext::GetBidirectionalRefine, &NAVCodecContext::SetBidirectionalRefine),
                R_ACCESSOR("minGopSize", &NAVCodecContext::GetMinGopSize, &NAVCodecContext::SetMinGopSize),
                R_ACCESSOR("referenceFrameCount", &NAVCodecContext::GetReferenceFrameCount, &NAVCodecContext::SetReferenceFrameCount),
                R_ACCESSOR("mv0Threshold", &NAVCodecContext::GetMv0Threshold, &NAVCodecContext::SetMv0Threshold),
                R_ACCESSOR("colorPrimaries", &NAVCodecContext::GetColorPrimaries, &NAVCodecContext::SetColorPrimaries),
                R_ACCESSOR("colorTrc", &NAVCodecContext::GetColorTrc, &NAVCodecContext::SetColorTrc),
                R_ACCESSOR("colorSpace", &NAVCodecContext::GetColorSpace, &NAVCodecContext::SetColorSpace),
                R_ACCESSOR("colorRange", &NAVCodecContext::GetColorRange, &NAVCodecContext::SetColorRange),
                R_ACCESSOR("chromeSampleLocation", &NAVCodecContext::GetChromeSampleLocation, &NAVCodecContext::SetChromeSampleLocation),
                R_ACCESSOR("sliceCount", &NAVCodecContext::GetSliceCount, &NAVCodecContext::SetSliceCount),
                R_ACCESSOR("fieldOrder", &NAVCodecContext::GetFieldOrder, &NAVCodecContext::SetFieldOrder),
                R_ACCESSOR("sampleRate", &NAVCodecContext::GetSampleRate, &NAVCodecContext::SetSampleRate),
                R_ACCESSOR("sampleFormat", &NAVCodecContext::GetSampleFormat, &NAVCodecContext::SetSampleFormat),
                R_ACCESSOR("frameSize", &NAVCodecContext::GetFrameSize, &NAVCodecContext::SetFrameSize),
                R_ACCESSOR("frameNumber", &NAVCodecContext::GetFrameNumber, &NAVCodecContext::SetFrameNumber),
                R_ACCESSOR("blockAlignment", &NAVCodecContext::GetBlockAlignment, &NAVCodecContext::SetBlockAlignment),
                R_ACCESSOR("cutoff", &NAVCodecContext::GetCutoff, &NAVCodecContext::SetCutoff),
                R_ACCESSOR("audioServiceType", &NAVCodecContext::GetAudioServiceType, &NAVCodecContext::SetAudioServiceType),
                R_ACCESSOR("requestSampleFormat", &NAVCodecContext::GetRequestSampleFormat, &NAVCodecContext::SetRequestSampleFormat),
                R_ACCESSOR("quantizerCompression", &NAVCodecContext::GetQuantizerCompression, &NAVCodecContext::SetQuantizerCompression),
                R_ACCESSOR("quantizerBlur", &NAVCodecContext::GetQuantizerBlur, &NAVCodecContext::SetQuantizerBlur),
                R_ACCESSOR("minQuantizer", &NAVCodecContext::GetMinQuantizer, &NAVCodecContext::SetMinQuantizer),
                R_ACCESSOR("maxQuantizer", &NAVCodecContext::GetMaxQuantizer, &NAVCodecContext::SetMaxQuantizer),
                R_ACCESSOR("maxQuantizerDifference", &NAVCodecContext::GetMaxQuantizerDifference, &NAVCodecContext::SetMaxQuantizerDifference),
                R_ACCESSOR("rateControlBufferSize", &NAVCodecContext::GetRateControlBufferSize, &NAVCodecContext::SetRateControlBufferSize),
                R_ACCESSOR("rateControlOverrideCount", &NAVCodecContext::GetRateControlOverrideCount, &NAVCodecContext::SetRateControlOverrideCount),
                R_ACCESSOR("rateControlMaxBitrate", &NAVCodecContext::GetRateControlMaxBitrate, &NAVCodecContext::SetRateControlMaxBitrate),
                R_ACCESSOR("rateControlMinBitrate", &NAVCodecContext::GetRateControlMinBitrate, &NAVCodecContext::SetRateControlMinBitrate),
                R_ACCESSOR("rateControlMaxAvailableVbvUse", &NAVCodecContext::GetRateControlMaxAvailableVbvUse, &NAVCodecContext::SetRateControlMaxAvailableVbvUse),
                R_ACCESSOR("rateControlMinVbvOverflowUse", &NAVCodecContext::GetRateControlMinVbvOverflowUse, &NAVCodecContext::SetRateControlMinVbvOverflowUse),
                R_ACCESSOR("rateControlInitialBufferOccupancy", &NAVCodecContext::GetRateControlInitialBufferOccupancy, &NAVCodecContext::SetRateControlInitialBufferOccupancy),
                R_ACCESSOR("trellis", &NAVCodecContext::GetTrellis, &NAVCodecContext::SetTrellis),
                R_ACCESSOR("workaroundBugs", &NAVCodecContext::GetWorkaroundBugs, &NAVCodecContext::SetWorkaroundBugs),
                R_ACCESSOR("errorRecognitionFlags", &NAVCodecContext::GetErrorRecognitionFlags, &NAVCodecContext::SetErrorRecognitionFlags)
            });
        }

        virtual void Free();

        void ThreadMain();

    private:
        bool FeedToCodec();
        bool PullFromDecoder(AVCodecContext *context);
        bool PullFromEncoder(AVCodecContext *context);
        
        // Threading infrastructure

        void WaitForWork();
        AVFrame *GetPoolFrame();
        void FreePoolFrame(AVFrame *frame);
        AVPacket *GetPoolPacket();
        void FreePoolPacket(AVPacket *frame);
        void SendError(std::string code, std::string message);
        void ThreadLog(std::string message);

        bool opened = false;
        bool threadTracing = false;
        std::queue<AVFrame*> framePool;
        std::queue<AVPacket*> packetPool;

        uint32_t framePoolHeight = 0;
        uint32_t packetPoolHeight = 0;
        
        std::thread *thread = nullptr;
        std::mutex mutex;
        std::condition_variable threadWake;
        std::deque<WorkItem> inQueue;
        std::queue<WorkItem> outQueue;
        bool running = true;

        Napi::FunctionReference onFrame;
        Napi::FunctionReference onPacket;
        Napi::FunctionReference onError;

        bool onFrameValid = false;
        bool onPacketValid = false; 
        bool onErrorValid = false;

        Napi::ThreadSafeFunction onFrameTSFN;
        Napi::ThreadSafeFunction onPacketTSFN;
        Napi::ThreadSafeFunction onErrorTSFN;

        // Functional

        Napi::Value Open(const Napi::CallbackInfo& info);
        Napi::Value SendPacket(const Napi::CallbackInfo& info);
        Napi::Value ReceiveFrame(const Napi::CallbackInfo& info);
        Napi::Value SendFrame(const Napi::CallbackInfo& info);
        Napi::Value ReceivePacket(const Napi::CallbackInfo& info);

        // Events

        Napi::Value GetOnFrame(const Napi::CallbackInfo& info);
        void SetOnFrame(const Napi::CallbackInfo& info, const Napi::Value &value);
        Napi::Value GetOnPacket(const Napi::CallbackInfo& info);
        void SetOnPacket(const Napi::CallbackInfo& info, const Napi::Value &value);
        Napi::Value GetOnError(const Napi::CallbackInfo& info);
        void SetOnError(const Napi::CallbackInfo& info, const Napi::Value &value);

        // Properties
        Napi::Value GetClass(const Napi::CallbackInfo& info);
        Napi::Value GetCodecType(const Napi::CallbackInfo& info);
        Napi::Value GetCodecId(const Napi::CallbackInfo& info);
        Napi::Value GetCodecTag(const Napi::CallbackInfo& info);
        Napi::Value GetBitRate(const Napi::CallbackInfo& info);
        void SetBitRate(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetBitRateTolerance(const Napi::CallbackInfo& info);
        void SetBitRateTolerance(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetGlobalQuality(const Napi::CallbackInfo& info);
        void SetGlobalQuality(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetCompressionLevel(const Napi::CallbackInfo& info);
        void SetCompressionLevel(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetFlags(const Napi::CallbackInfo& info);
        void SetFlags(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetFlags2(const Napi::CallbackInfo& info);
        void SetFlags2(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetTimeBase(const Napi::CallbackInfo& info);
        void SetTimeBase(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetTicksPerFrame(const Napi::CallbackInfo& info);
        void SetTicksPerFrame(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetDelay(const Napi::CallbackInfo& info);
        void SetDelay(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetWidth(const Napi::CallbackInfo& info);
        void SetWidth(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetHeight(const Napi::CallbackInfo& info);
        void SetHeight(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetCodedWidth(const Napi::CallbackInfo& info);
        void SetCodedWidth(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetCodedHeight(const Napi::CallbackInfo& info);
        void SetCodedHeight(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetGopSize(const Napi::CallbackInfo& info);
        void SetGopSize(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetPixelFormat(const Napi::CallbackInfo& info);
        void SetPixelFormat(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetMaxBFrames(const Napi::CallbackInfo& info);
        void SetMaxBFrames(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetBQuantizationFactor(const Napi::CallbackInfo& info);
        void SetBQuantizationFactor(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetBQuantizationOffset(const Napi::CallbackInfo& info);
        void SetBQuantizationOffset(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetHasBFrames(const Napi::CallbackInfo& info);
        void SetHasBFrames(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetIQuantizationFactor(const Napi::CallbackInfo& info);
        void SetIQuantizationFactor(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetIQuantizationOffset(const Napi::CallbackInfo& info);
        void SetIQuantizationOffset(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetLuminanceMasking(const Napi::CallbackInfo& info);
        void SetLuminanceMasking(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetTemporalComplexityMasking(const Napi::CallbackInfo& info);
        void SetTemporalComplexityMasking(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetSpatialComplexityMasking(const Napi::CallbackInfo& info);
        void SetSpatialComplexityMasking(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetPMasking(const Napi::CallbackInfo& info);
        void SetPMasking(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetDarkMasking(const Napi::CallbackInfo& info);
        void SetDarkMasking(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetSampleAspectRatio(const Napi::CallbackInfo& info);
        void SetSampleAspectRatio(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetMotionEstimationComparisonFunction(const Napi::CallbackInfo& info);
        void SetMotionEstimationComparisonFunction(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetSubpixelMotionEstimationComparisonFunction(const Napi::CallbackInfo& info);
        void SetSubpixelMotionEstimationComparisonFunction(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetMacroblockComparisonFunction(const Napi::CallbackInfo& info);
        void SetMacroblockComparisonFunction(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetInterlacedDCTComparisonFunction(const Napi::CallbackInfo& info);
        void SetInterlacedDCTComparisonFunction(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetDiamondSize(const Napi::CallbackInfo& info);
        void SetDiamondSize(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetLastPredictorCount(const Napi::CallbackInfo& info);
        void SetLastPredictorCount(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetMotionEstimationPrepassComparisonFunction(const Napi::CallbackInfo& info);
        void SetMotionEstimationPrepassComparisonFunction(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetMotionEstimationPrepassDiamondSize(const Napi::CallbackInfo& info);
        void SetMotionEstimationPrepassDiamondSize(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetMotionEstimationSubpelQuality(const Napi::CallbackInfo& info);
        void SetMotionEstimationSubpelQuality(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetMotionEstimationRange(const Napi::CallbackInfo& info);
        void SetMotionEstimationRange(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetSliceFlags(const Napi::CallbackInfo& info);
        void SetSliceFlags(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetMacroblockDecisionMode(const Napi::CallbackInfo& info);
        void SetMacroblockDecisionMode(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetIntraDCPrecision(const Napi::CallbackInfo& info);
        void SetIntraDCPrecision(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetSkipTop(const Napi::CallbackInfo& info);
        void SetSkipTop(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetSkipBottom(const Napi::CallbackInfo& info);
        void SetSkipBottom(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetMinMBLagrangeMultiplier(const Napi::CallbackInfo& info);
        void SetMinMBLagrangeMultiplier(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetMaxLBLagrangeMultiplier(const Napi::CallbackInfo& info);
        void SetMaxLBLagrangeMultiplier(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetBidirectionalRefine(const Napi::CallbackInfo& info);
        void SetBidirectionalRefine(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetMinGopSize(const Napi::CallbackInfo& info);
        void SetMinGopSize(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetReferenceFrameCount(const Napi::CallbackInfo& info);
        void SetReferenceFrameCount(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetMv0Threshold(const Napi::CallbackInfo& info);
        void SetMv0Threshold(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetColorPrimaries(const Napi::CallbackInfo& info);
        void SetColorPrimaries(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetColorTrc(const Napi::CallbackInfo& info);
        void SetColorTrc(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetColorSpace(const Napi::CallbackInfo& info);
        void SetColorSpace(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetColorRange(const Napi::CallbackInfo& info);
        void SetColorRange(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetChromeSampleLocation(const Napi::CallbackInfo& info);
        void SetChromeSampleLocation(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetSliceCount(const Napi::CallbackInfo& info);
        void SetSliceCount(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetFieldOrder(const Napi::CallbackInfo& info);
        void SetFieldOrder(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetSampleRate(const Napi::CallbackInfo& info);
        void SetSampleRate(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetSampleFormat(const Napi::CallbackInfo& info);
        void SetSampleFormat(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetFrameSize(const Napi::CallbackInfo& info);
        void SetFrameSize(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetFrameNumber(const Napi::CallbackInfo& info);
        void SetFrameNumber(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetBlockAlignment(const Napi::CallbackInfo& info);
        void SetBlockAlignment(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetCutoff(const Napi::CallbackInfo& info);
        void SetCutoff(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetAudioServiceType(const Napi::CallbackInfo& info);
        void SetAudioServiceType(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetRequestSampleFormat(const Napi::CallbackInfo& info);
        void SetRequestSampleFormat(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetQuantizerCompression(const Napi::CallbackInfo& info);
        void SetQuantizerCompression(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetQuantizerBlur(const Napi::CallbackInfo& info);
        void SetQuantizerBlur(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetMinQuantizer(const Napi::CallbackInfo& info);
        void SetMinQuantizer(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetMaxQuantizer(const Napi::CallbackInfo& info);
        void SetMaxQuantizer(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetMaxQuantizerDifference(const Napi::CallbackInfo& info);
        void SetMaxQuantizerDifference(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetRateControlBufferSize(const Napi::CallbackInfo& info);
        void SetRateControlBufferSize(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetRateControlOverrideCount(const Napi::CallbackInfo& info);
        void SetRateControlOverrideCount(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetRateControlMaxBitrate(const Napi::CallbackInfo& info);
        void SetRateControlMaxBitrate(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetRateControlMinBitrate(const Napi::CallbackInfo& info);
        void SetRateControlMinBitrate(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetRateControlMaxAvailableVbvUse(const Napi::CallbackInfo& info);
        void SetRateControlMaxAvailableVbvUse(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetRateControlMinVbvOverflowUse(const Napi::CallbackInfo& info);
        void SetRateControlMinVbvOverflowUse(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetRateControlInitialBufferOccupancy(const Napi::CallbackInfo& info);
        void SetRateControlInitialBufferOccupancy(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetTrellis(const Napi::CallbackInfo& info);
        void SetTrellis(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetWorkaroundBugs(const Napi::CallbackInfo& info);
        void SetWorkaroundBugs(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetErrorRecognitionFlags(const Napi::CallbackInfo& info);
        void SetErrorRecognitionFlags(const Napi::CallbackInfo& info, const Napi::Value& value);

        Napi::Value GetChannelLayout(const Napi::CallbackInfo& info);
        void SetChannelLayout(const Napi::CallbackInfo& info, const Napi::Value& value);
};

void nlavc_thread(NAVCodecContext *context);