/*
 * AVCodec public API
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

import { AVClass, AVHWDeviceType, AVMediaType, AVPixelFormat, AVRational, AVSampleFormat } from "../avutil";
import { AVCodecContext } from "./avcodec";
import { AVCodecID } from "./codec_id";

/**
 * @addtogroup lavc_core
 * @{
 */

/**
 * Decoder can use draw_horiz_band callback.
 */
export const AV_CODEC_CAP_DRAW_HORIZ_BAND =     (1 <<  0);
/**
 * Codec uses get_buffer() or get_encode_buffer() for allocating buffers and
 * supports custom allocators.
 * If not set, it might not use get_buffer() or get_encode_buffer() at all, or
 * use operations that assume the buffer was allocated by
 * avcodec_default_get_buffer2 or avcodec_default_get_encode_buffer.
 */
export const AV_CODEC_CAP_DR1 =                 (1 <<  1);

/**
 * @deprecated Use parsers to always send proper frames.
 * @if FF_API_FLAG_TRUNCATED
 */
export const AV_CODEC_CAP_TRUNCATED =           (1 <<  3);

/**
 * Encoder or decoder requires flushing with NULL input at the end in order to
 * give the complete and correct output.
 *
 * NOTE: If this flag is not set, the codec is guaranteed to never be fed with
 *       with NULL data. The user can still send NULL data to the public encode
 *       or decode function, but libavcodec will not pass it along to the codec
 *       unless this flag is set.
 *
 * Decoders:
 * The decoder has a non-zero delay and needs to be fed with avpkt->data=NULL,
 * avpkt->size=0 at the end to get the delayed data until the decoder no longer
 * returns frames.
 *
 * Encoders:
 * The encoder needs to be fed with NULL data at the end of encoding until the
 * encoder no longer returns data.
 *
 * NOTE: For encoders implementing the AVCodec.encode2() function, setting this
 *       flag also means that the encoder must set the pts and duration for
 *       each output packet. If this flag is not set, the pts and duration will
 *       be determined by libavcodec from the input frame.
 */
export const AV_CODEC_CAP_DELAY =               (1 <<  5);
/**
 * Codec can be fed a final frame with a smaller size.
 * This can be used to prevent truncation of the last audio samples.
 */
export const AV_CODEC_CAP_SMALL_LAST_FRAME =    (1 <<  6);

/**
 * Codec can output multiple frames per AVPacket
 * Normally demuxers return one frame at a time, demuxers which do not do
 * are connected to a parser to split what they return into proper frames.
 * This flag is reserved to the very rare category of codecs which have a
 * bitstream that cannot be split into frames without timeconsuming
 * operations like full decoding. Demuxers carrying such bitstreams thus
 * may return multiple frames in a packet. This has many disadvantages like
 * prohibiting stream copy in many cases thus it should only be considered
 * as a last resort.
 */
export const AV_CODEC_CAP_SUBFRAMES =           (1 <<  8);
/**
 * Codec is experimental and is thus avoided in favor of non experimental
 * encoders
 */
export const AV_CODEC_CAP_EXPERIMENTAL =        (1 <<  9);
/**
 * Codec should fill in channel configuration and samplerate instead of container
 */
export const AV_CODEC_CAP_CHANNEL_CONF =        (1 << 10);
/**
 * Codec supports frame-level multithreading.
 */
export const AV_CODEC_CAP_FRAME_THREADS =       (1 << 12);
/**
 * Codec supports slice-based (or partition-based) multithreading.
 */
export const AV_CODEC_CAP_SLICE_THREADS =       (1 << 13);
/**
 * Codec supports changed parameters at any point.
 */
export const AV_CODEC_CAP_PARAM_CHANGE =        (1 << 14);
/**
 * Codec supports multithreading through a method other than slice- or
 * frame-level multithreading. Typically this marks wrappers around
 * multithreading-capable external libraries.
 */
export const AV_CODEC_CAP_OTHER_THREADS =       (1 << 15);

/**
 * @if FF_API_AUTO_THREADS
 */
export const AV_CODEC_CAP_AUTO_THREADS =        AV_CODEC_CAP_OTHER_THREADS;

/**
 * Audio encoder supports receiving a different number of samples in each call.
 */
export const AV_CODEC_CAP_VARIABLE_FRAME_SIZE = (1 << 16);
/**
 * Decoder is not a preferred choice for probing.
 * This indicates that the decoder is not a good choice for probing.
 * It could for example be an expensive to spin up hardware decoder,
 * or it could simply not provide a lot of useful information about
 * the stream.
 * A decoder marked with this flag should only be used as last resort
 * choice for probing.
 */
export const AV_CODEC_CAP_AVOID_PROBING =       (1 << 17);

/**
 * Deprecated and unused. Use AVCodecDescriptor.props instead
 * @if FF_API_UNUSED_CODEC_CAPS
 */
export const AV_CODEC_CAP_INTRA_ONLY =       0x40000000;
/**
 * Deprecated and unused. Use AVCodecDescriptor.props instead
 * @if FF_API_UNUSED_CODEC_CAPS
 */
export const AV_CODEC_CAP_LOSSLESS =         0x80000000;

/**
 * Codec is backed by a hardware implementation. Typically used to
 * identify a non-hwaccel hardware decoder. For information about hwaccels, use
 * avcodec_get_hw_config() instead.
 */
export const AV_CODEC_CAP_HARDWARE =            (1 << 18);

/**
 * Codec is potentially backed by a hardware implementation, but not
 * necessarily. This is used instead of AV_CODEC_CAP_HARDWARE, if the
 * implementation provides some sort of internal fallback.
 */
export const AV_CODEC_CAP_HYBRID =              (1 << 19);

/**
 * This codec takes the reordered_opaque field from input AVFrames
 * and returns it in the corresponding field in AVCodecContext after
 * encoding.
 */
export const AV_CODEC_CAP_ENCODER_REORDERED_OPAQUE = (1 << 20);

/**
 * This encoder can be flushed using avcodec_flush_buffers(). If this flag is
 * not set, the encoder must be closed and reopened to ensure that no frames
 * remain pending.
 */
export const AV_CODEC_CAP_ENCODER_FLUSH =   (1 << 21);

/**
 * AVProfile.
 */
export declare class AVProfile {
    readonly profile: number;
    readonly name: string; ///< short name for the profile
}

/**
 * AVCodec.
 */
export declare class AVCodec {
    static readonly version: number;
    static readonly configuration: string;
    static readonly license: string;

    static all(): AVCodec[];
    static findDecoder(name: string): AVCodec;
    static findDecoder(id: AVCodecID): AVCodec;
    static findEncoder(name: string): AVCodec;
    static findEncoder(id: AVCodecID): AVCodec;

    newContext(): AVCodecContext;

    /**
     * Name of the codec implementation.
     * The name is globally unique among encoders and among decoders (but an
     * encoder and a decoder can share the same name).
     * This is the primary way to find a codec from the user perspective.
     */
    readonly name: string;

    /**
     * Descriptive name for the codec, meant to be more human readable than name.
     * You should use the NULL_IF_CONFIG_SMALL() macro to define it.
     */
    readonly longName: string;
    readonly type: AVMediaType;
    readonly id: AVCodecID;

    /**
     * Codec capabilities.
     * see AV_CODEC_CAP_*
     */
    readonly capabilities: number;
    readonly decoderMaxLowResolution: number;                     ///< maximum value for lowres supported by the decoder
    readonly supportedFrameRates: AVRational[]; ///< array of supported framerates, or NULL if any, array is terminated by {0,0}
    readonly pixelFormats: AVPixelFormat[];     ///< array of supported pixel formats, or NULL if unknown, array is terminated by -1
    readonly supportedSampleRates: number[];       ///< array of supported audio samplerates, or NULL if unknown, array is terminated by 0
    readonly sampleFormats: AVSampleFormat[]; ///< array of supported sample formats, or NULL if unknown, array is terminated by -1
    readonly privateClass: AVClass;              ///< AVClass for the private context
    readonly profiles: AVProfile[];              ///< array of recognized profiles, or NULL if unknown, array is terminated by {FF_PROFILE_UNKNOWN}

    /**
     * Group name of the codec implementation.
     * This is a short symbolic name of the wrapper backing this codec. A
     * wrapper uses some kind of external implementation for the codec, such
     * as an external library, or a codec implementation provided by the OS or
     * the hardware.
     * If this field is NULL, this is a builtin, libavcodec native codec.
     * If non-NULL, this will be the suffix in AVCodec.name in most cases
     * (usually AVCodec.name will be of the form "<codec_name>_<wrapper_name>").
     */
    readonly wrapperName: string;

    /**
     * Array of supported channel layouts, terminated with a zeroed layout.
     */
    readonly channelLayouts: number[];

    readonly isEncoder: boolean;
    readonly isDecoder: boolean;

    getProfile(profile: number): AVProfile;
}

/**
 * The codec supports this format via the hw_device_ctx interface.
 *
 * When selecting this format, AVCodecContext.hw_device_ctx should
 * have been set to a device of the specified type before calling
 * avcodec_open2().
 */
export const AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX = 0x01;

/**
 * The codec supports this format via the hw_frames_ctx interface.
 *
 * When selecting this format for a decoder,
 * AVCodecContext.hw_frames_ctx should be set to a suitable frames
 * context inside the get_format() callback.  The frames context
 * must have been created on a device of the specified type.
 *
 * When selecting this format for an encoder,
 * AVCodecContext.hw_frames_ctx should be set to the context which
 * will be used for the input frames before calling avcodec_open2().
 */
export const AV_CODEC_HW_CONFIG_METHOD_HW_FRAMES_CTX = 0x02;
/**
 * The codec supports this format by some internal method.
 *
 * This format can be selected without any additional configuration -
 * no device or frames context is required.
 */
export const AV_CODEC_HW_CONFIG_METHOD_INTERNAL      = 0x04;
/**
 * The codec supports this format by some ad-hoc method.
 *
 * Additional settings and/or function calls are required.  See the
 * codec-specific documentation for details.  (Methods requiring
 * this sort of configuration are deprecated and others should be
 * used in preference.)
 */
export const AV_CODEC_HW_CONFIG_METHOD_AD_HOC        = 0x08;

export interface AVCodecHWConfig {
    /**
     * For decoders, a hardware pixel format which that decoder may be
     * able to decode to if suitable hardware is available.
     *
     * For encoders, a pixel format which the encoder may be able to
     * accept.  If set to AV_PIX_FMT_NONE, this applies to all pixel
     * formats supported by the codec.
     */
    pix_fmt: AVPixelFormat;
    /**
     * Bit set of AV_CODEC_HW_CONFIG_METHOD_* flags, describing the possible
     * setup methods which can be used with this configuration.
     */
    methods: number;
    /**
     * The device type associated with the configuration.
     *
     * Must be set for AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX and
     * AV_CODEC_HW_CONFIG_METHOD_HW_FRAMES_CTX, otherwise unused.
     */
    device_type: AVHWDeviceType;
}