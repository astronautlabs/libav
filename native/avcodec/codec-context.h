#include "../common.h"

#include <napi.h>
#include "../resource.h"

extern "C" {
    #include <libavcodec/avcodec.h>
}

class NAVCodecContext : public NAVResource<NAVCodecContext, AVCodecContext> {
    public:
        NAVCodecContext(const Napi::CallbackInfo& info);
        
        inline static std::string ExportName() { return "AVCodecContext"; }
        inline static Napi::Function ClassDefinition(const Napi::Env &env) {
            return DefineClass(env, "AVCodecContext", {
                R_GETTER("GetClass", &NAVCodecContext::GetClass),
                R_GETTER("GetCodecType", &NAVCodecContext::GetCodecType),
                R_GETTER("GetCodecId", &NAVCodecContext::GetCodecId),
                R_GETTER("GetCodecTag", &NAVCodecContext::GetCodecTag),
                R_GETTER("GetBitRate", &NAVCodecContext::GetBitRate),
                R_GETTER("GetBitRateTolerance", &NAVCodecContext::GetBitRateTolerance),
                R_GETTER("GetGlobalQuality", &NAVCodecContext::GetGlobalQuality),
                R_GETTER("GetCompressionLevel", &NAVCodecContext::GetCompressionLevel),
                R_GETTER("GetFlags", &NAVCodecContext::GetFlags),
                R_GETTER("GetFlags2", &NAVCodecContext::GetFlags2),
                R_GETTER("GetTimeBase", &NAVCodecContext::GetTimeBase),
                R_GETTER("GetTicksPerFrame", &NAVCodecContext::GetTicksPerFrame),
                R_GETTER("GetDelay", &NAVCodecContext::GetDelay),
                R_GETTER("GetWidth", &NAVCodecContext::GetWidth),
                R_GETTER("GetHeight", &NAVCodecContext::GetHeight),
                R_GETTER("GetCodedWidth", &NAVCodecContext::GetCodedWidth),
                R_GETTER("GetCodedHeight", &NAVCodecContext::GetCodedHeight),
                R_GETTER("GetGopSize", &NAVCodecContext::GetGopSize),
                R_GETTER("GetPixelFormat", &NAVCodecContext::GetPixelFormat),
                R_GETTER("GetFormat", &NAVCodecContext::GetFormat),
                R_GETTER("GetMaxBFrames", &NAVCodecContext::GetMaxBFrames),
                R_GETTER("GetBQuantizationFactor", &NAVCodecContext::GetBQuantizationFactor),
                R_GETTER("GetBQuantizationOffset", &NAVCodecContext::GetBQuantizationOffset),
                R_GETTER("GetBFrames", &NAVCodecContext::GetBFrames),
                R_GETTER("GetIQuantizationFactor", &NAVCodecContext::GetIQuantizationFactor),
                R_GETTER("GetIQuantizationOffset", &NAVCodecContext::GetIQuantizationOffset),
                R_GETTER("GetLuminanceMasking", &NAVCodecContext::GetLuminanceMasking),
                R_GETTER("GetTemporalComplexityMasking", &NAVCodecContext::GetTemporalComplexityMasking),
                R_GETTER("GetSpatialComplexityMasking", &NAVCodecContext::GetSpatialComplexityMasking),
                R_GETTER("GetPMasking", &NAVCodecContext::GetPMasking),
                R_GETTER("GetDarkMasking", &NAVCodecContext::GetDarkMasking),
                R_GETTER("GetSampleAspectRatio", &NAVCodecContext::GetSampleAspectRatio),
                R_GETTER("GetMotionEstimationComparisonFunction", &NAVCodecContext::GetMotionEstimationComparisonFunction),
                R_GETTER("GetSubpixelMotionEstimationComparisonFunction", &NAVCodecContext::GetSubpixelMotionEstimationComparisonFunction),
                R_GETTER("GetMacroblockComparisonFunction", &NAVCodecContext::GetMacroblockComparisonFunction),
                R_GETTER("GetInterlacedDCTComparisonFunction", &NAVCodecContext::GetInterlacedDCTComparisonFunction),
                R_GETTER("GetDiamondSize", &NAVCodecContext::GetDiamondSize),
                R_GETTER("GetLastPredictorCount", &NAVCodecContext::GetLastPredictorCount),
                R_GETTER("GetMotionEstimationPrepassComparisonFunction", &NAVCodecContext::GetMotionEstimationPrepassComparisonFunction),
                R_GETTER("GetMotionEstimationPrepassDiamondSize", &NAVCodecContext::GetMotionEstimationPrepassDiamondSize),
                R_GETTER("GetMotionEstimationSubpelQuality", &NAVCodecContext::GetMotionEstimationSubpelQuality),
                R_GETTER("GetMotionEstimationRange", &NAVCodecContext::GetMotionEstimationRange),
                R_GETTER("GetSliceFlags", &NAVCodecContext::GetSliceFlags),
                R_GETTER("GetSkipTop", &NAVCodecContext::GetSkipTop),
                R_GETTER("GetSkipBottom", &NAVCodecContext::GetSkipBottom),
                R_GETTER("MinMBLagrangeMultiplier", &NAVCodecContext::MinMBLagrangeMultiplier),
                R_GETTER("MaxLBLagrangeMultiplier", &NAVCodecContext::MaxLBLagrangeMultiplier),
                R_GETTER("GetBidirectionalRefine", &NAVCodecContext::GetBidirectionalRefine),
                R_GETTER("GetMinGopSize", &NAVCodecContext::GetMinGopSize),
                R_GETTER("GetReferenceFrameCount", &NAVCodecContext::GetReferenceFrameCount),
                R_GETTER("GetMv0Threshold", &NAVCodecContext::GetMv0Threshold),
                R_GETTER("GetColorPrimaries", &NAVCodecContext::GetColorPrimaries),
                R_GETTER("GetColorTrc", &NAVCodecContext::GetColorTrc),
                R_GETTER("GetColorSpace", &NAVCodecContext::GetColorSpace),
                R_GETTER("GetColorRange", &NAVCodecContext::GetColorRange),
                R_GETTER("GetChromeSampleLocation", &NAVCodecContext::GetChromeSampleLocation),
                R_GETTER("GetSliceCount", &NAVCodecContext::GetSliceCount),
                R_GETTER("GetFieldOrder", &NAVCodecContext::GetFieldOrder),
                R_GETTER("GetSampleRate", &NAVCodecContext::GetSampleRate),
                R_GETTER("GetSampleFormat", &NAVCodecContext::GetSampleFormat),
                R_GETTER("GetFrameSize", &NAVCodecContext::GetFrameSize),
                R_GETTER("GetFrameNumber", &NAVCodecContext::GetFrameNumber),
                R_GETTER("GetBlockAlignment", &NAVCodecContext::GetBlockAlignment),
                R_GETTER("GetCutoff", &NAVCodecContext::GetCutoff),
                R_GETTER("GetAudioServiceType", &NAVCodecContext::GetAudioServiceType),
                R_GETTER("GetRequestSampleFormat", &NAVCodecContext::GetRequestSampleFormat),
                R_GETTER("GetQuantizerCompression", &NAVCodecContext::GetQuantizerCompression),
                R_GETTER("GetQuantizerBlur", &NAVCodecContext::GetQuantizerBlur),
                R_GETTER("GetMinQuantizer", &NAVCodecContext::GetMinQuantizer),
                R_GETTER("GetMaxQuantizer", &NAVCodecContext::GetMaxQuantizer),
                R_GETTER("GetMaxQuantizerDifference", &NAVCodecContext::GetMaxQuantizerDifference),
                R_GETTER("GetRateControlBufferSize", &NAVCodecContext::GetRateControlBufferSize),
                R_GETTER("GetRateControlOverrideCount", &NAVCodecContext::GetRateControlOverrideCount),
                R_GETTER("GetRateControlMaxBitrate", &NAVCodecContext::GetRateControlMaxBitrate),
                R_GETTER("GetRateControlMinBitrate", &NAVCodecContext::GetRateControlMinBitrate),
                R_GETTER("GetRateControlMaxAvailableVbvSize", &NAVCodecContext::GetRateControlMaxAvailableVbvSize),
                R_GETTER("GetRateControlMinVbvOverflowUse", &NAVCodecContext::GetRateControlMinVbvOverflowUse),
                R_GETTER("GetRateControlInitialBufferOccupancy", &NAVCodecContext::GetRateControlInitialBufferOccupancy),
                R_GETTER("GetTrellis", &NAVCodecContext::GetTrellis),
                R_GETTER("GetWorkaroundBugs", &NAVCodecContext::GetWorkaroundBugs),
                R_GETTER("GetErrorRecognitionFlags", &NAVCodecContext::GetErrorRecognitionFlags)
            });
        }

        virtual void Free();
    private:

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
        Napi::Value GetFormat(const Napi::CallbackInfo& info);
        Napi::Value GetMaxBFrames(const Napi::CallbackInfo& info);
        Napi::Value GetBQuantizationFactor(const Napi::CallbackInfo& info);
        Napi::Value GetBQuantizationOffset(const Napi::CallbackInfo& info);
        Napi::Value GetBFrames(const Napi::CallbackInfo& info);
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
        Napi::Value GetSkipTop(const Napi::CallbackInfo& info);
        Napi::Value GetSkipBottom(const Napi::CallbackInfo& info);
        Napi::Value MinMBLagrangeMultiplier(const Napi::CallbackInfo& info);
        Napi::Value MaxLBLagrangeMultiplier(const Napi::CallbackInfo& info);
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
        Napi::Value GetRateControlMaxAvailableVbvSize(const Napi::CallbackInfo& info);
        Napi::Value GetRateControlMinVbvOverflowUse(const Napi::CallbackInfo& info);
        Napi::Value GetRateControlInitialBufferOccupancy(const Napi::CallbackInfo& info);
        Napi::Value GetTrellis(const Napi::CallbackInfo& info);
        Napi::Value GetWorkaroundBugs(const Napi::CallbackInfo& info);
        Napi::Value GetErrorRecognitionFlags(const Napi::CallbackInfo& info);
};