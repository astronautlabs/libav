#include "codec-context.h"
#include "codec.h"

NAVCodecContext::NAVCodecContext(const Napi::CallbackInfo& info):
    NAVResource(info)
{
    if (ConstructFromHandle(info))
        return;
    
    auto codec = NAVCodec::Unwrap(info[0].As<Napi::Object>());
    auto handle = avcodec_alloc_context3(codec->GetHandle());
    SetHandle(handle);
}

void NAVCodecContext::Free() {
    auto handle = GetHandle();
    avcodec_free_context(&handle);
    SetHandle(handle);
}



Napi::Value NAVCodecContext::GetClass(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetCodecType(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetCodecId(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetCodecTag(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetBitRate(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetBitRateTolerance(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetGlobalQuality(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetCompressionLevel(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetFlags(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetFlags2(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetTimeBase(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetTicksPerFrame(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetDelay(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetWidth(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetHeight(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetCodedWidth(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetCodedHeight(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetGopSize(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetPixelFormat(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetFormat(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetMaxBFrames(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetBQuantizationFactor(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetBQuantizationOffset(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetBFrames(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetIQuantizationFactor(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetIQuantizationOffset(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetLuminanceMasking(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetTemporalComplexityMasking(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetSpatialComplexityMasking(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetPMasking(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetDarkMasking(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetSampleAspectRatio(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetMotionEstimationComparisonFunction(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetSubpixelMotionEstimationComparisonFunction(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetMacroblockComparisonFunction(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetInterlacedDCTComparisonFunction(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetDiamondSize(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetLastPredictorCount(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetMotionEstimationPrepassComparisonFunction(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetMotionEstimationPrepassDiamondSize(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetMotionEstimationSubpelQuality(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetMotionEstimationRange(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetSliceFlags(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetSkipTop(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetSkipBottom(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::MinMBLagrangeMultiplier(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::MaxLBLagrangeMultiplier(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetBidirectionalRefine(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetMinGopSize(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetReferenceFrameCount(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetMv0Threshold(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetColorPrimaries(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetColorTrc(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetColorSpace(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetColorRange(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetChromeSampleLocation(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetSliceCount(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetFieldOrder(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetSampleRate(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetSampleFormat(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetFrameSize(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetFrameNumber(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetBlockAlignment(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetCutoff(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetAudioServiceType(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetRequestSampleFormat(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetQuantizerCompression(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetQuantizerBlur(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetMinQuantizer(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetMaxQuantizer(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetMaxQuantizerDifference(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetRateControlBufferSize(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetRateControlOverrideCount(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetRateControlMaxBitrate(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetRateControlMinBitrate(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetRateControlMaxAvailableVbvSize(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetRateControlMinVbvOverflowUse(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetRateControlInitialBufferOccupancy(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetTrellis(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetWorkaroundBugs(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}

Napi::Value NAVCodecContext::GetErrorRecognitionFlags(const Napi::CallbackInfo& info) {
    return info.Env().Undefined(); // TODO
}
