/*
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

import { AVPictureType } from "./avutil";
import { AVBuffer } from "./buffer";
import { AVDictionary } from "./dict";
import { AVChromaLocation, AVColorPrimaries, AVColorRange, AVColorSpace, AVColorTransferCharacteristic } from "./pixfmt";
import { AVRational } from "./rational";

/**
 * @file
 * @ingroup lavu_frame
 * reference-counted frame API
 */

/**
 * @defgroup lavu_frame AVFrame
 * @ingroup lavu_data
 *
 * @{
 * AVFrame is an abstraction for reference-counted raw multimedia data.
 */

export enum AVFrameSideDataType {
    /**
     * The data is the AVPanScan struct defined in libavcodec.
     */
    AV_FRAME_DATA_PANSCAN,
    /**
     * ATSC A53 Part 4 Closed Captions.
     * A53 CC bitstream is stored as uint8_t in AVFrameSideData.data.
     * The number of bytes of CC data is AVFrameSideData.size.
     */
    AV_FRAME_DATA_A53_CC,
    /**
     * Stereoscopic 3d metadata.
     * The data is the AVStereo3D struct defined in libavutil/stereo3d.h.
     */
    AV_FRAME_DATA_STEREO3D,
    /**
     * The data is the AVMatrixEncoding enum defined in libavutil/channel_layout.h.
     */
    AV_FRAME_DATA_MATRIXENCODING,
    /**
     * Metadata relevant to a downmix procedure.
     * The data is the AVDownmixInfo struct defined in libavutil/downmix_info.h.
     */
    AV_FRAME_DATA_DOWNMIX_INFO,
    /**
     * ReplayGain information in the form of the AVReplayGain struct.
     */
    AV_FRAME_DATA_REPLAYGAIN,
    /**
     * This side data contains a 3x3 transformation matrix describing an affine
     * transformation that needs to be applied to the frame for correct
     * presentation.
     *
     * See libavutil/display.h for a detailed description of the data.
     */
    AV_FRAME_DATA_DISPLAYMATRIX,
    /**
     * Active Format Description data consisting of a single byte as specified
     * in ETSI TS 101 154 using AVActiveFormatDescription enum.
     */
    AV_FRAME_DATA_AFD,
    /**
     * Motion vectors exported by some codecs (on demand through the export_mvs
     * flag set in the libavcodec AVCodecContext flags2 option).
     * The data is the AVMotionVector struct defined in
     * libavutil/motion_vector.h.
     */
    AV_FRAME_DATA_MOTION_VECTORS,
    /**
     * Recommmends skipping the specified number of samples. This is exported
     * only if the "skip_manual" AVOption is set in libavcodec.
     * This has the same format as AV_PKT_DATA_SKIP_SAMPLES.
     * @code
     * u32le number of samples to skip from start of this packet
     * u32le number of samples to skip from end of this packet
     * u8    reason for start skip
     * u8    reason for end   skip (0=padding silence, 1=convergence)
     * @endcode
     */
    AV_FRAME_DATA_SKIP_SAMPLES,
    /**
     * This side data must be associated with an audio frame and corresponds to
     * enum AVAudioServiceType defined in avcodec.h.
     */
    AV_FRAME_DATA_AUDIO_SERVICE_TYPE,
    /**
     * Mastering display metadata associated with a video frame. The payload is
     * an AVMasteringDisplayMetadata type and contains information about the
     * mastering display color volume.
     */
    AV_FRAME_DATA_MASTERING_DISPLAY_METADATA,
    /**
     * The GOP timecode in 25 bit timecode format. Data format is 64-bit integer.
     * This is set on the first frame of a GOP that has a temporal reference of 0.
     */
    AV_FRAME_DATA_GOP_TIMECODE,

    /**
     * The data represents the AVSphericalMapping structure defined in
     * libavutil/spherical.h.
     */
    AV_FRAME_DATA_SPHERICAL,

    /**
     * Content light level (based on CTA-861.3). This payload contains data in
     * the form of the AVContentLightMetadata struct.
     */
    AV_FRAME_DATA_CONTENT_LIGHT_LEVEL,

    /**
     * The data contains an ICC profile as an opaque octet buffer following the
     * format described by ISO 15076-1 with an optional name defined in the
     * metadata key entry "name".
     */
    AV_FRAME_DATA_ICC_PROFILE,

    /**
     * Timecode which conforms to SMPTE ST 12-1. The data is an array of 4 uint32_t
     * where the first uint32_t describes how many (1-3) of the other timecodes are used.
     * The timecode format is described in the documentation of av_timecode_get_smpte_from_framenum()
     * function in libavutil/timecode.h.
     */
    AV_FRAME_DATA_S12M_TIMECODE,

    /**
     * HDR dynamic metadata associated with a video frame. The payload is
     * an AVDynamicHDRPlus type and contains information for color
     * volume transform - application 4 of SMPTE 2094-40:2016 standard.
     */
    AV_FRAME_DATA_DYNAMIC_HDR_PLUS,

    /**
     * Regions Of Interest, the data is an array of AVRegionOfInterest type, the number of
     * array element is implied by AVFrameSideData.size / AVRegionOfInterest.self_size.
     */
    AV_FRAME_DATA_REGIONS_OF_INTEREST,

    /**
     * Encoding parameters for a video frame, as described by AVVideoEncParams.
     */
    AV_FRAME_DATA_VIDEO_ENC_PARAMS,

    /**
     * User data unregistered metadata associated with a video frame.
     * This is the H.26[45] UDU SEI message, and shouldn't be used for any other purpose
     * The data is stored as uint8_t in AVFrameSideData.data which is 16 bytes of
     * uuid_iso_iec_11578 followed by AVFrameSideData.size - 16 bytes of user_data_payload_byte.
     */
    AV_FRAME_DATA_SEI_UNREGISTERED,

    /**
     * Film grain parameters for a frame, described by AVFilmGrainParams.
     * Must be present for every frame which should have film grain applied.
     */
    AV_FRAME_DATA_FILM_GRAIN_PARAMS,

    /**
     * Bounding boxes for object detection and classification,
     * as described by AVDetectionBBoxHeader.
     */
    AV_FRAME_DATA_DETECTION_BBOXES,

    /**
     * Dolby Vision RPU raw data, suitable for passing to x265
     * or other libraries. Array of uint8_t, with NAL emulation
     * bytes intact.
     */
    AV_FRAME_DATA_DOVI_RPU_BUFFER,

    /**
     * Parsed Dolby Vision metadata, suitable for passing to a software
     * implementation. The payload is the AVDOVIMetadata struct defined in
     * libavutil/dovi_meta.h.
     */
    AV_FRAME_DATA_DOVI_METADATA,

    /**
     * HDR Vivid dynamic metadata associated with a video frame. The payload is
     * an AVDynamicHDRVivid type and contains information for color
     * volume transform - CUVA 005.1-2021.
     */
    AV_FRAME_DATA_DYNAMIC_HDR_VIVID,
};

export enum AVActiveFormatDescription {
    AV_AFD_SAME         = 8,
    AV_AFD_4_3          = 9,
    AV_AFD_16_9         = 10,
    AV_AFD_14_9         = 11,
    AV_AFD_4_3_SP_14_9  = 13,
    AV_AFD_16_9_SP_14_9 = 14,
    AV_AFD_SP_4_3       = 15,
};


/**
 * Structure to hold side data for an AVFrame.
 *
 * sizeof(AVFrameSideData) is not a part of the public ABI, so new fields may be added
 * to the end with a minor bump.
 */
export declare class AVFrameSideData {
    readonly type: AVFrameSideDataType;
    readonly size: number;
    readonly metadata: AVDictionary;
    readonly buf: AVBuffer;
}

/**
 * Structure describing a single Region Of Interest.
 *
 * When multiple regions are defined in a single side-data block, they
 * should be ordered from most to least important - some encoders are only
 * capable of supporting a limited number of distinct regions, so will have
 * to truncate the list.
 *
 * When overlapping regions are defined, the first region containing a given
 * area of the frame applies.
 */
export interface AVRegionOfInterest {
    /**
     * Must be set to the size of this data structure (that is,
     * sizeof(AVRegionOfInterest)).
     */
    self_size: number;
    /**
     * Distance in pixels from the top edge of the frame to the top and
     * bottom edges and from the left edge of the frame to the left and
     * right edges of the rectangle defining this region of interest.
     *
     * The constraints on a region are encoder dependent, so the region
     * actually affected may be slightly larger for alignment or other
     * reasons.
     */
    top: number;
    bottom: number;
    left: number;
    right: number;
    /**
     * Quantisation offset.
     *
     * Must be in the range -1 to +1.  A value of zero indicates no quality
     * change.  A negative value asks for better quality (less quantisation),
     * while a positive value asks for worse quality (greater quantisation).
     *
     * The range is calibrated so that the extreme values indicate the
     * largest possible offset - if the rest of the frame is encoded with the
     * worst possible quality, an offset of -1 indicates that this region
     * should be encoded with the best possible quality anyway.  Intermediate
     * values are then interpolated in some codec-dependent way.
     *
     * For example, in 10-bit H.264 the quantisation parameter varies between
     * -12 and 51.  A typical qoffset value of -1/10 therefore indicates that
     * this region should be encoded with a QP around one-tenth of the full
     * range better than the rest of the frame.  So, if most of the frame
     * were to be encoded with a QP of around 30, this region would get a QP
     * of around 24 (an offset of approximately -1/10 * (51 - -12) = -6.3).
     * An extreme value of -1 would indicate that this region should be
     * encoded with the best possible quality regardless of the treatment of
     * the rest of the frame - that is, should be encoded at a QP of -12.
     */
    qoffset: AVRational;
}

export const AV_NUM_DATA_POINTERS = 8;

/**
 * @defgroup lavu_frame_flags AV_FRAME_FLAGS
 * @ingroup lavu_frame
 * Flags describing additional frame properties.
 *
 * @{
 */

/**
 * The frame data may be corrupted, e.g. due to decoding errors.
 */
export const AV_FRAME_FLAG_CORRUPT =       (1 << 0);
/**
 * A flag to mark the frames which need to be decoded, but shouldn't be output.
 */
export const AV_FRAME_FLAG_DISCARD =   (1 << 2);
/**
 * @}
 */

export const FF_DECODE_ERROR_INVALID_BITSTREAM =   1;
export const FF_DECODE_ERROR_MISSING_REFERENCE =   2;
export const FF_DECODE_ERROR_CONCEALMENT_ACTIVE =  4;
export const FF_DECODE_ERROR_DECODE_SLICES =       8;
 
/**
 * This structure describes decoded (raw) audio or video data.
 *
 * AVFrame must be allocated using av_frame_alloc(). Note that this only
 * allocates the AVFrame itself, the buffers for the data must be managed
 * through other means (see below).
 * AVFrame must be freed with av_frame_free().
 *
 * AVFrame is typically allocated once and then reused multiple times to hold
 * different data (e.g. a single AVFrame to hold frames received from a
 * decoder). In such a case, av_frame_unref() will free any references held by
 * the frame and reset it to its original clean state before it
 * is reused again.
 *
 * The data described by an AVFrame is usually reference counted through the
 * AVBuffer API. The underlying buffer references are stored in AVFrame.buf /
 * AVFrame.extended_buf. An AVFrame is considered to be reference counted if at
 * least one reference is set, i.e. if AVFrame.buf[0] != NULL. In such a case,
 * every single data plane must be contained in one of the buffers in
 * AVFrame.buf or AVFrame.extended_buf.
 * There may be a single buffer for all the data, or one separate buffer for
 * each plane, or anything in between.
 *
 * sizeof(AVFrame) is not a part of the public ABI, so new fields may be added
 * to the end with a minor bump.
 *
 * Fields can be accessed through AVOptions, the name string used, matches the
 * C structure field name for fields accessible through AVOptions. The AVClass
 * for AVFrame can be obtained from avcodec_get_frame_class()
 */
export declare class AVFrame {
    static getSideDataName(type: AVFrameSideDataType): string;

    constructor();
    /**
     * Set up a new reference to the data described by the source frame.
     *
     * Copy frame properties from src to dst and create a new reference for each
     * AVBufferRef from src.
     *
     * If src is not reference counted, new buffers are allocated and the data is
     * copied.
     *
     * @warning: This frame MUST have been either unreferenced or newly constructed
     *          before calling this function, or undefined behavior will occur.
     */
    referTo(sourceFrame: AVFrame);

    /**
     * Unreference all the buffers referenced by frame and reset the frame fields.
     */
    unrefer();

    /**
     * Create a new frame that references the same data as this one.
     * This is a shortcut for new AVFrame().referTo(this)
     */
    clone(): AVFrame;

    /**
     * Copy the frame data from this frame to another frame.
     *
     * This function does not allocate anything, the other frame must be already 
     * initialized and allocated with the same parameters as this one.
     *
     * This function only copies the frame data (i.e. the contents of the data /
     * extended data arrays), not any other properties.
     */
    copyTo(other: AVFrame);

    /**
     * Copy only "metadata" fields from this frame to another.
     *
     * Metadata for the purpose of this function are those fields that do not affect
     * the data layout in the buffers.  E.g. pts, sample rate (for audio) or sample
     * aspect ratio (for video), but not width/height or channel layout.
     * Side data is also copied.
     */
    copyPropertiesTo(other: AVFrame);

    /**
     * Get the buffer reference a given data plane is stored in.
     *
     * @param plane index of the data plane of interest in frame->extendedBuffers.
     * @return the buffer reference that contains the plane.
     */
    getPlaneBuffer(plane: number): AVBuffer;

    /**
     * Add a new side data to the frame.
     *
     * @param type type of the added side data
     * @param size size of the side data
     *
     * @return newly added side data object
     */
    addSideData(type: AVFrameSideDataType, buffer: AVBuffer): AVFrameSideData;
    addSideData(type: AVFrameSideDataType, size: number): AVFrameSideData;

    /**
     * Retrieve the side data object with the given type, if it exists.
     * Otherwise null is returned.
     */
    getSideData(type: AVFrameSideDataType): AVFrameSideData;

    /**
     * Remove the side data object with the given type, if it exists.
     */
    removeSideData(type: AVFrameSideDataType);

    /**
     * Crop this video frame according to its cropLeft/cropTop/cropRight/
     * cropBottom fields. If cropping is successful, the function will adjust the
     * data pointers and the width/height fields, and set the crop fields to 0.
     *
     * In all cases, the cropping boundaries will be rounded to the inherent
     * alignment of the pixel format. In some cases, such as for opaque hwaccel
     * formats, the left/top cropping is ignored. The crop fields are set to 0 even
     * if the cropping was rounded or ignored.
     * 
     * If the cropping fields were invalid, ERANGE is thrown and nothing is changed.
     *
     * @param flags Some combination of AV_FRAME_CROP_* flags, or 0 (default).
     */
    applyCropping(flags?: number);

    /**
     * Move everything contained in the other frame into this one and 
     * reset the other frame.
     *
     * @warning: The other frame is not unreferenced, but directly overwritten 
     *           without reading or deallocating its contents. Call 
     *           unrefer() manually before calling this function to ensure 
     *           that no memory is leaked.
     */
    moveReferenceFrom(other: AVFrame);

    /**
     * Allocate new buffer(s) for audio or video data.
     *
     * The following fields must be set on frame before calling this function:
     * - format (pixel format for video, sample format for audio)
     * - width and height for video
     * - numberOfSamples and channelLayout for audio
     *
     * This function will fill `buffers` and, if necessary, `extendedBuffers`.
     * For planar formats, one buffer will be allocated for each plane.
     *
     * @warning: if frame's buffers have already been allocated, calling this function will
     *           leak memory. In addition, undefined behavior can occur in certain
     *           cases.
     *
     * @param align Required buffer size alignment. If equal to 0, alignment will be
     *              chosen automatically for the current CPU. It is highly
     *              recommended to pass 0 (default) here unless you know what you are doing.
     */
    allocateBuffer(alignment?: number);

    /**
     * Check if the frame data is writable.
     *
     * @return True if the frame data is writable (which is the case if and
     * only if each of the underlying buffers has only one reference, namely the one
     * stored in this frame). Return 0 otherwise.
     *
     * If `true` is returned the answer is valid until av_buffer_ref() is called on any
     * of the underlying AVBufferRefs (e.g. through av_frame_ref() or directly).
     */
    readonly writable: boolean;

    /**
     * NOTE: Arrays returned by this property are a copy of the values in the AVFrame. 
     * You must set this field entirely to change it. If you pass an array with less than 
     * AV_NUM_DATA_POINTERS elements, the remaining ones will be set to zero.
     * 
     * For video, a positive or negative value, which is typically indicating
     * the size in bytes of each picture line, but it can also be:
     * - the negative byte size of lines for vertical flipping
     *   (with data[n] pointing to the end of the data
     * - a positive or negative multiple of the byte size as for accessing
     *   even and odd fields of a frame (possibly flipped)
     *
     * For audio, only linesize[0] may be set. For planar audio, each channel
     * plane must be the same size.
     *
     * For video the linesizes should be multiples of the CPUs alignment
     * preference, this is 16 or 32 for modern desktop CPUs.
     * Some code requires such alignment other code can be slower without
     * correct alignment, for yet other it makes no difference.
     *
     * @note The linesize may be larger than the size of usable data -- there
     * may be extra padding present for performance reasons.
     *
     * @attention In case of video, line size values can be negative to achieve
     * a vertically inverted iteration over image lines.
     * 
     * @note Length is AV_NUM_DATA_POINTERS
     */
    lineSizes: number[];

    /**
     * @name Video dimensions
     * Video frames only. The coded dimensions (in pixels) of the video frame,
     * i.e. the size of the rectangle that contains some well-defined values.
     *
     * @note The part of the frame intended for display/presentation is further
     * restricted by the @ref cropping "Cropping rectangle".
     * @{
     */
    width: number;
    height: number;

    /**
     * @}
     */

    /**
     * number of audio samples (per channel) described by this frame
     */
    numberOfSamples: number;

    /**
     * format of the frame, -1 if unknown or unset
     * Values correspond to enum AVPixelFormat for video frames,
     * enum AVSampleFormat for audio)
     */
    format: number;

    /**
     * Whether this is a key frame or not
     */
    keyFrame: boolean;

    /**
     * Picture type of the frame.
     */
    pictureType: AVPictureType;

    /**
     * Sample aspect ratio for the video frame, 0/1 if unknown/unspecified.
     */
    sampleAspectRatio: AVRational;

    /**
     * Presentation timestamp in time_base units (time when frame should be shown to user).
     */
    pts: number;

    /**
     * DTS copied from the AVPacket that triggered returning this frame. (if frame threading isn't used)
     * This is also the Presentation time of this AVFrame calculated from
     * only AVPacket.dts values without pts values.
     */
    packetDts: number;

    /**
     * Time base for the timestamps in this frame.
     * In the future, this field may be set on frames output by decoders or
     * filters, but its value will be by default ignored on input to encoders
     * or filters.
     */
    timeBase: AVRational;

    /**
     * picture number in bitstream order
     */
    codedPictureNumber: number;
    /**
     * picture number in display order
     */
    displayPictureNumber: number;

    /**
     * quality (between 1 (good) and FF_LAMBDA_MAX (bad))
     */
    quality: number;

    /**
     * When decoding, this signals how much the picture must be delayed.
     * extra_delay = repeat_pict / (2*fps)
     */
    repeatPicture: number;

    /**
     * The content of the picture is interlaced.
     */
    interlaced: boolean;

    /**
     * If the content is interlaced, is top field displayed first.
     */
    topFieldFirst: boolean;

    /**
     * Tell user application that palette has changed from previous frame.
     */
    paletteHasChanged: number;

    /**
     * reordered opaque 64 bits (generally an integer or a double precision float
     * PTS but can be anything).
     * The user sets AVCodecContext.reordered_opaque to represent the input at
     * that time,
     * the decoder reorders values as needed and sets AVFrame.reordered_opaque
     * to exactly one of the values provided by the user through AVCodecContext.reordered_opaque
     */
    reorderedOpaque: number;

    /**
     * Sample rate of the audio data.
     */
    sampleRate: number;

    /**
     * AVBuffer references backing the data for this frame. All the pointers in
     * data and extended_data must point inside one of the buffers in buf or
     * extended_buf. This array must be filled contiguously -- if buf[i] is
     * non-NULL then buf[j] must also be non-NULL for all j < i.
     *
     * There may be at most one AVBuffer per data plane, so for video this array
     * always contains all the references. For planar audio with more than
     * AV_NUM_DATA_POINTERS channels, there may be more buffers than can fit in
     * this array. Then the extra AVBufferRef pointers are stored in the
     * extended_buf array.
     * 
     * @note Length is AV_NUM_DATA_POINTERS
     */
    buffers: AVBuffer[];

    /**
     * For planar audio which requires more than AV_NUM_DATA_POINTERS
     * AVBufferRef pointers, this array will hold all the references which
     * cannot fit into AVFrame.buf.
     *
     * Note that this is different from AVFrame.extended_data, which always
     * contains all the pointers. This array only contains the extra pointers,
     * which cannot fit into AVFrame.buf.
     *
     * This array is always allocated using av_malloc() by whoever constructs
     * the frame. It is freed in av_frame_unref().
     */
    extendedBuffers: AVBuffer[];

    /**
     * Retrieve the full set of side data objects.
     */
    readonly sideData: AVFrameSideData[];

    /**
     * Frame flags, a combination of @ref lavu_frame_flags
     */
    flags: number;

    /**
     * MPEG vs JPEG YUV range.
     * - encoding: Set by user
     * - decoding: Set by libavcodec
     */
    colorRange: AVColorRange;

    colorPrimaries: AVColorPrimaries;

    colorTrc: AVColorTransferCharacteristic;

    /**
     * YUV colorspace type.
     * - encoding: Set by user
     * - decoding: Set by libavcodec
     */
    colorSpace: AVColorSpace;

    chromaLocation: AVChromaLocation;

    /**
     * frame timestamp estimated using various heuristics, in stream time base
     * - encoding: unused
     * - decoding: set by libavcodec, read by user.
     */
    bestEffortTimestamp: number;

    /**
     * reordered pos from the last AVPacket that has been input into the decoder
     * - encoding: unused
     * - decoding: Read by user.
     */
    packetPosition: number;

    /**
     * duration of the corresponding packet, expressed in
     * AVStream->time_base units, 0 if unknown.
     * - encoding: unused
     * - decoding: Read by user.
     */
    packetDuration: number;

    /**
     * metadata.
     * - encoding: Set by user.
     * - decoding: Set by libavcodec.
     */
    metadata: AVDictionary;

    /**
     * decode error flags of the frame, set to a combination of
     * FF_DECODE_ERROR_xxx flags if the decoder produced a frame, but there
     * were errors during the decoding.
     * - encoding: unused
     * - decoding: set by libavcodec, read by user.
     */
    decodeErrorFlags: number;

    /**
     * size of the corresponding packet containing the compressed
     * frame.
     * It is set to a negative value if unknown.
     * - encoding: unused
     * - decoding: set by libavcodec, read by user.
     */
    packetSize: number;

    /**
     * For hwaccel-format frames, this should be a reference to the
     * AVHWFramesContext describing the frame.
     */
    hwFramesContext: AVBuffer;

    /**
     * AVBufferRef for free use by the API user. FFmpeg will never check the
     * contents of the buffer ref. FFmpeg calls av_buffer_unref() on it when
     * the frame is unreferenced. av_frame_copy_props() calls create a new
     * reference with av_buffer_ref() for the target frame's opaque_ref field.
     *
     * This is unrelated to the opaque field, although it serves a similar
     * purpose.
     */
    opaqueRef: AVBuffer;

    /**
     * @anchor cropping
     * @name Cropping
     * Video frames only. The number of pixels to discard from the the
     * top/bottom/left/right border of the frame to obtain the sub-rectangle of
     * the frame intended for presentation.
     * @{
     */
    cropTop: number;
    cropBottom: number;
    cropLeft: number;
    cropRight: number;

    /**
     * Channel layout of the audio data.
     */
    channelLayout: number;
}

/**
 * Flags for frame cropping.
 */

/**
 * Apply the maximum possible cropping, even if it requires setting the
 * AVFrame.data[] entries to unaligned pointers. Passing unaligned data
 * to FFmpeg API is generally not allowed, and causes undefined behavior
 * (such as crashes). You can pass unaligned data only to FFmpeg APIs that
 * are explicitly documented to accept it. Use this flag only if you
 * absolutely know what you are doing.
 */
export const AV_FRAME_CROP_UNALIGNED     = 1 << 0;
