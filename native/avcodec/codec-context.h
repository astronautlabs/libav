#include "../common.h"

#include <napi.h>
#include "../resource.h"
#include <thread>
#include <queue>

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

                R_GETTER("class", &NAVCodecContext::GetClass),
                R_GETTER("codecType", &NAVCodecContext::GetCodecType),
                R_GETTER("codecID", &NAVCodecContext::GetCodecId),
                R_GETTER("codecTag", &NAVCodecContext::GetCodecTag),
                R_GETTER("bitRate", &NAVCodecContext::GetBitRate),
                R_GETTER("bitRateTolerance", &NAVCodecContext::GetBitRateTolerance),
                R_GETTER("globalQuality", &NAVCodecContext::GetGlobalQuality),
                R_GETTER("compressionLevel", &NAVCodecContext::GetCompressionLevel),
                R_GETTER("clags", &NAVCodecContext::GetFlags),
                R_GETTER("clags2", &NAVCodecContext::GetFlags2),
                R_GETTER("timeBase", &NAVCodecContext::GetTimeBase),
                R_GETTER("ticksPerFrame", &NAVCodecContext::GetTicksPerFrame),
                R_GETTER("delay", &NAVCodecContext::GetDelay),
                R_GETTER("width", &NAVCodecContext::GetWidth),
                R_GETTER("height", &NAVCodecContext::GetHeight),
                R_GETTER("codedWidth", &NAVCodecContext::GetCodedWidth),
                R_GETTER("codedHeight", &NAVCodecContext::GetCodedHeight),
                R_GETTER("gopSize", &NAVCodecContext::GetGopSize),
                R_GETTER("pixelFormat", &NAVCodecContext::GetPixelFormat),
                R_GETTER("maxBFrames", &NAVCodecContext::GetMaxBFrames),
                R_GETTER("bQuantizationFactor", &NAVCodecContext::GetBQuantizationFactor),
                R_GETTER("bQuantizationOffset", &NAVCodecContext::GetBQuantizationOffset),
                R_GETTER("hasBFrames", &NAVCodecContext::HasBFrames),
                R_GETTER("iQuantizationFactor", &NAVCodecContext::GetIQuantizationFactor),
                R_GETTER("iQuantizationOffset", &NAVCodecContext::GetIQuantizationOffset),
                R_GETTER("luminanceMasking", &NAVCodecContext::GetLuminanceMasking),
                R_GETTER("temporalComplexityMasking", &NAVCodecContext::GetTemporalComplexityMasking),
                R_GETTER("spatialComplexityMasking", &NAVCodecContext::GetSpatialComplexityMasking),
                R_GETTER("pMasking", &NAVCodecContext::GetPMasking),
                R_GETTER("darkMasking", &NAVCodecContext::GetDarkMasking),
                R_GETTER("sampleAspectRatio", &NAVCodecContext::GetSampleAspectRatio),
                R_GETTER("motionEstimationComparisonFunction", &NAVCodecContext::GetMotionEstimationComparisonFunction),
                R_GETTER("subpixelMotionEstimationComparisonFunction", &NAVCodecContext::GetSubpixelMotionEstimationComparisonFunction),
                R_GETTER("macroblockComparisonFunction", &NAVCodecContext::GetMacroblockComparisonFunction),
                R_GETTER("interlacedDCTComparisonFunction", &NAVCodecContext::GetInterlacedDCTComparisonFunction),
                R_GETTER("diamondSize", &NAVCodecContext::GetDiamondSize),
                R_GETTER("lastPredictorCount", &NAVCodecContext::GetLastPredictorCount),
                R_GETTER("motionEstimationPrepassComparisonFunction", &NAVCodecContext::GetMotionEstimationPrepassComparisonFunction),
                R_GETTER("motionEstimationPrepassDiamondSize", &NAVCodecContext::GetMotionEstimationPrepassDiamondSize),
                R_GETTER("motionEstimationSubpelQuality", &NAVCodecContext::GetMotionEstimationSubpelQuality),
                R_GETTER("motionEstimationRange", &NAVCodecContext::GetMotionEstimationRange),
                R_GETTER("sliceFlags", &NAVCodecContext::GetSliceFlags),
                R_GETTER("macroblockDecisionMode", &NAVCodecContext::GetMacroblockDecisionMode),
                R_GETTER("intraDCPrecision", &NAVCodecContext::GetIntraDCPrecision),
                R_GETTER("skipTop", &NAVCodecContext::GetSkipTop),
                R_GETTER("skipBottom", &NAVCodecContext::GetSkipBottom),
                R_GETTER("minMBLagrangeMultiplier", &NAVCodecContext::GetMinMBLagrangeMultiplier),
                R_GETTER("maxLBLagrangeMultiplier", &NAVCodecContext::GetMaxLBLagrangeMultiplier),
                R_GETTER("bidirectionalRefine", &NAVCodecContext::GetBidirectionalRefine),
                R_GETTER("minGopSize", &NAVCodecContext::GetMinGopSize),
                R_GETTER("referenceFrameCount", &NAVCodecContext::GetReferenceFrameCount),
                R_GETTER("mv0Threshold", &NAVCodecContext::GetMv0Threshold),
                R_GETTER("colorPrimaries", &NAVCodecContext::GetColorPrimaries),
                R_GETTER("colorTrc", &NAVCodecContext::GetColorTrc),
                R_GETTER("colorSpace", &NAVCodecContext::GetColorSpace),
                R_GETTER("colorRange", &NAVCodecContext::GetColorRange),
                R_GETTER("chromeSampleLocation", &NAVCodecContext::GetChromeSampleLocation),
                R_GETTER("sliceCount", &NAVCodecContext::GetSliceCount),
                R_GETTER("fieldOrder", &NAVCodecContext::GetFieldOrder),
                R_GETTER("sampleRate", &NAVCodecContext::GetSampleRate),
                R_GETTER("sampleFormat", &NAVCodecContext::GetSampleFormat),
                R_GETTER("frameSize", &NAVCodecContext::GetFrameSize),
                R_GETTER("frameNumber", &NAVCodecContext::GetFrameNumber),
                R_GETTER("blockAlignment", &NAVCodecContext::GetBlockAlignment),
                R_GETTER("cutoff", &NAVCodecContext::GetCutoff),
                R_GETTER("audioServiceType", &NAVCodecContext::GetAudioServiceType),
                R_GETTER("requestSampleFormat", &NAVCodecContext::GetRequestSampleFormat),
                R_GETTER("quantizerCompression", &NAVCodecContext::GetQuantizerCompression),
                R_GETTER("quantizerBlur", &NAVCodecContext::GetQuantizerBlur),
                R_GETTER("minQuantizer", &NAVCodecContext::GetMinQuantizer),
                R_GETTER("maxQuantizer", &NAVCodecContext::GetMaxQuantizer),
                R_GETTER("maxQuantizerDifference", &NAVCodecContext::GetMaxQuantizerDifference),
                R_GETTER("rateControlBufferSize", &NAVCodecContext::GetRateControlBufferSize),
                R_GETTER("rateControlOverrideCount", &NAVCodecContext::GetRateControlOverrideCount),
                R_GETTER("rateControlMaxBitrate", &NAVCodecContext::GetRateControlMaxBitrate),
                R_GETTER("rateControlMinBitrate", &NAVCodecContext::GetRateControlMinBitrate),
                R_GETTER("rateControlMaxAvailableVbvUse", &NAVCodecContext::GetRateControlMaxAvailableVbvUse),
                R_GETTER("rateControlMinVbvOverflowUse", &NAVCodecContext::GetRateControlMinVbvOverflowUse),
                R_GETTER("rateControlInitialBufferOccupancy", &NAVCodecContext::GetRateControlInitialBufferOccupancy),
                R_GETTER("trellis", &NAVCodecContext::GetTrellis),
                R_GETTER("workaroundBugs", &NAVCodecContext::GetWorkaroundBugs),
                R_GETTER("errorRecognitionFlags", &NAVCodecContext::GetErrorRecognitionFlags)
            });
        }

        virtual void Free();

        void ThreadMain();

    private:
        bool FeedToCodec();
        bool PullFromDecoder(AVCodecContext *context);
        bool PullFromEncoder(AVCodecContext *context);
        
        void WaitForWork();
        AVFrame *GetPoolFrame();
        void FreePoolFrame(AVFrame *frame);
        AVPacket *GetPoolPacket();
        void FreePoolPacket(AVPacket *frame);

        std::queue<AVFrame*> framePool;
        std::queue<AVPacket*> packetPool;

        int framePoolHeight = 0;
        int packetPoolHeight = 0;
        
        std::thread *thread = nullptr;
        std::mutex mutex;
        std::condition_variable threadWake;
        std::deque<WorkItem> inQueue;
        std::queue<WorkItem> outQueue;
        bool running = true;

        Napi::FunctionReference onFrame;
        Napi::FunctionReference onPacket;

        Napi::ThreadSafeFunction onFrameTSFN;
        Napi::ThreadSafeFunction onPacketTSFN;

        // Functional

        Napi::Value SendPacket(const Napi::CallbackInfo& info);
        Napi::Value ReceiveFrame(const Napi::CallbackInfo& info);
        Napi::Value SendFrame(const Napi::CallbackInfo& info);
        Napi::Value ReceivePacket(const Napi::CallbackInfo& info);

        // Properties

        Napi::Value GetOnFrame(const Napi::CallbackInfo& info);
        void SetOnFrame(const Napi::CallbackInfo& info, const Napi::Value &value);
        Napi::Value GetOnPacket(const Napi::CallbackInfo& info);
        void SetOnPacket(const Napi::CallbackInfo& info, const Napi::Value &value);

        Napi::Value GetClass(const Napi::CallbackInfo& info);
        Napi::Value GetCodecType(const Napi::CallbackInfo& info);
        Napi::Value GetCodecId(const Napi::CallbackInfo& info);
        Napi::Value GetCodecTag(const Napi::CallbackInfo& info);
        Napi::Value GetBitRate(const Napi::CallbackInfo& info);
        Napi::Value GetBitRateTolerance(const Napi::CallbackInfo& info);
        Napi::Value GetGlobalQuality(const Napi::CallbackInfo& info);
        Napi::Value GetCompressionLevel(const Napi::CallbackInfo& info);
        Napi::Value GetFlags(const Napi::CallbackInfo& info);
        Napi::Value GetFlags2(const Napi::CallbackInfo& info);
        Napi::Value GetTimeBase(const Napi::CallbackInfo& info);
        Napi::Value GetTicksPerFrame(const Napi::CallbackInfo& info);
        Napi::Value GetDelay(const Napi::CallbackInfo& info);
        Napi::Value GetWidth(const Napi::CallbackInfo& info);
        Napi::Value GetHeight(const Napi::CallbackInfo& info);
        Napi::Value GetCodedWidth(const Napi::CallbackInfo& info);
        Napi::Value GetCodedHeight(const Napi::CallbackInfo& info);
        Napi::Value GetGopSize(const Napi::CallbackInfo& info);
        Napi::Value GetPixelFormat(const Napi::CallbackInfo& info);
        Napi::Value GetMaxBFrames(const Napi::CallbackInfo& info);
        Napi::Value GetBQuantizationFactor(const Napi::CallbackInfo& info);
        Napi::Value GetBQuantizationOffset(const Napi::CallbackInfo& info);
        Napi::Value HasBFrames(const Napi::CallbackInfo& info);
        Napi::Value GetIQuantizationFactor(const Napi::CallbackInfo& info);
        Napi::Value GetIQuantizationOffset(const Napi::CallbackInfo& info);
        Napi::Value GetLuminanceMasking(const Napi::CallbackInfo& info);
        Napi::Value GetTemporalComplexityMasking(const Napi::CallbackInfo& info);
        Napi::Value GetSpatialComplexityMasking(const Napi::CallbackInfo& info);
        Napi::Value GetPMasking(const Napi::CallbackInfo& info);
        Napi::Value GetDarkMasking(const Napi::CallbackInfo& info);
        Napi::Value GetSampleAspectRatio(const Napi::CallbackInfo& info);
        Napi::Value GetMotionEstimationComparisonFunction(const Napi::CallbackInfo& info);
        Napi::Value GetSubpixelMotionEstimationComparisonFunction(const Napi::CallbackInfo& info);
        Napi::Value GetMacroblockComparisonFunction(const Napi::CallbackInfo& info);
        Napi::Value GetInterlacedDCTComparisonFunction(const Napi::CallbackInfo& info);
        Napi::Value GetDiamondSize(const Napi::CallbackInfo& info);
        Napi::Value GetLastPredictorCount(const Napi::CallbackInfo& info);
        Napi::Value GetMotionEstimationPrepassComparisonFunction(const Napi::CallbackInfo& info);
        Napi::Value GetMotionEstimationPrepassDiamondSize(const Napi::CallbackInfo& info);
        Napi::Value GetMotionEstimationSubpelQuality(const Napi::CallbackInfo& info);
        Napi::Value GetMotionEstimationRange(const Napi::CallbackInfo& info);
        Napi::Value GetSliceFlags(const Napi::CallbackInfo& info);
        Napi::Value GetMacroblockDecisionMode(const Napi::CallbackInfo& info);
        Napi::Value GetIntraDCPrecision(const Napi::CallbackInfo& info);
        Napi::Value GetSkipTop(const Napi::CallbackInfo& info);
        Napi::Value GetSkipBottom(const Napi::CallbackInfo& info);
        Napi::Value GetMinMBLagrangeMultiplier(const Napi::CallbackInfo& info);
        Napi::Value GetMaxLBLagrangeMultiplier(const Napi::CallbackInfo& info);
        Napi::Value GetBidirectionalRefine(const Napi::CallbackInfo& info);
        Napi::Value GetMinGopSize(const Napi::CallbackInfo& info);
        Napi::Value GetReferenceFrameCount(const Napi::CallbackInfo& info);
        Napi::Value GetMv0Threshold(const Napi::CallbackInfo& info);
        Napi::Value GetColorPrimaries(const Napi::CallbackInfo& info);
        Napi::Value GetColorTrc(const Napi::CallbackInfo& info);
        Napi::Value GetColorSpace(const Napi::CallbackInfo& info);
        Napi::Value GetColorRange(const Napi::CallbackInfo& info);
        Napi::Value GetChromeSampleLocation(const Napi::CallbackInfo& info);
        Napi::Value GetSliceCount(const Napi::CallbackInfo& info);
        Napi::Value GetFieldOrder(const Napi::CallbackInfo& info);
        Napi::Value GetSampleRate(const Napi::CallbackInfo& info);
        Napi::Value GetSampleFormat(const Napi::CallbackInfo& info);
        Napi::Value GetFrameSize(const Napi::CallbackInfo& info);
        Napi::Value GetFrameNumber(const Napi::CallbackInfo& info);
        Napi::Value GetBlockAlignment(const Napi::CallbackInfo& info);
        Napi::Value GetCutoff(const Napi::CallbackInfo& info);
        Napi::Value GetAudioServiceType(const Napi::CallbackInfo& info);
        Napi::Value GetRequestSampleFormat(const Napi::CallbackInfo& info);
        Napi::Value GetQuantizerCompression(const Napi::CallbackInfo& info);
        Napi::Value GetQuantizerBlur(const Napi::CallbackInfo& info);
        Napi::Value GetMinQuantizer(const Napi::CallbackInfo& info);
        Napi::Value GetMaxQuantizer(const Napi::CallbackInfo& info);
        Napi::Value GetMaxQuantizerDifference(const Napi::CallbackInfo& info);
        Napi::Value GetRateControlBufferSize(const Napi::CallbackInfo& info);
        Napi::Value GetRateControlOverrideCount(const Napi::CallbackInfo& info);
        Napi::Value GetRateControlMaxBitrate(const Napi::CallbackInfo& info);
        Napi::Value GetRateControlMinBitrate(const Napi::CallbackInfo& info);
        Napi::Value GetRateControlMaxAvailableVbvUse(const Napi::CallbackInfo& info);
        Napi::Value GetRateControlMinVbvOverflowUse(const Napi::CallbackInfo& info);
        Napi::Value GetRateControlInitialBufferOccupancy(const Napi::CallbackInfo& info);
        Napi::Value GetTrellis(const Napi::CallbackInfo& info);
        Napi::Value GetWorkaroundBugs(const Napi::CallbackInfo& info);
        Napi::Value GetErrorRecognitionFlags(const Napi::CallbackInfo& info);
};

void nlavc_thread(NAVCodecContext *context);