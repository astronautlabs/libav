/*
* Copyright (c) 2006 Michael Niedermayer <michaelni@gmx.at>
* Copyright (c) 2008 Peter Ross
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

import { NotImplemented, OpaquePtr, Ref } from "../helpers"

/**
 * @file
 * audio channel layout utility functions
 */

/**
 * @addtogroup lavu_audio
 * @{
 */

export enum AVChannel {
    ///< Invalid channel index
    AV_CHAN_NONE = -1,
    AV_CHAN_FRONT_LEFT,
    AV_CHAN_FRONT_RIGHT,
    AV_CHAN_FRONT_CENTER,
    AV_CHAN_LOW_FREQUENCY,
    AV_CHAN_BACK_LEFT,
    AV_CHAN_BACK_RIGHT,
    AV_CHAN_FRONT_LEFT_OF_CENTER,
    AV_CHAN_FRONT_RIGHT_OF_CENTER,
    AV_CHAN_BACK_CENTER,
    AV_CHAN_SIDE_LEFT,
    AV_CHAN_SIDE_RIGHT,
    AV_CHAN_TOP_CENTER,
    AV_CHAN_TOP_FRONT_LEFT,
    AV_CHAN_TOP_FRONT_CENTER,
    AV_CHAN_TOP_FRONT_RIGHT,
    AV_CHAN_TOP_BACK_LEFT,
    AV_CHAN_TOP_BACK_CENTER,
    AV_CHAN_TOP_BACK_RIGHT,
    /** Stereo downmix. */
    AV_CHAN_STEREO_LEFT = 29,
    /** See above. */
    AV_CHAN_STEREO_RIGHT,
    AV_CHAN_WIDE_LEFT,
    AV_CHAN_WIDE_RIGHT,
    AV_CHAN_SURROUND_DIRECT_LEFT,
    AV_CHAN_SURROUND_DIRECT_RIGHT,
    AV_CHAN_LOW_FREQUENCY_2,
    AV_CHAN_TOP_SIDE_LEFT,
    AV_CHAN_TOP_SIDE_RIGHT,
    AV_CHAN_BOTTOM_FRONT_CENTER,
    AV_CHAN_BOTTOM_FRONT_LEFT,
    AV_CHAN_BOTTOM_FRONT_RIGHT,

    /** Channel is empty can be safely skipped. */
    AV_CHAN_UNUSED = 0x200,

    /** Channel contains data, but its position is unknown. */
    AV_CHAN_UNKNOWN = 0x300,

    /**
     * Range of channels between AV_CHAN_AMBISONIC_BASE and
     * AV_CHAN_AMBISONIC_END represent Ambisonic components using the ACN system.
     *
     * Given a channel id <i> between AV_CHAN_AMBISONIC_BASE and
     * AV_CHAN_AMBISONIC_END (inclusive), the ACN index of the channel <n> is
     * <n> = <i> - AV_CHAN_AMBISONIC_BASE.
     *
     * @note these values are only used for AV_CHANNEL_ORDER_CUSTOM channel
     * orderings, the AV_CHANNEL_ORDER_AMBISONIC ordering orders the channels
     * implicitly by their position in the stream.
     */
    AV_CHAN_AMBISONIC_BASE = 0x400,
    // leave space for 1024 ids, which correspond to maximum order-32 harmonics,
    // which should be enough for the foreseeable use cases
    AV_CHAN_AMBISONIC_END  = 0x7ff,
};

export enum AVChannelOrder {
    /**
     * Only the channel count is specified, without any further information
     * about the channel order.
     */
    AV_CHANNEL_ORDER_UNSPEC,
    /**
     * The native channel order, i.e. the channels are in the same order in
     * which they are defined in the AVChannel enum. This supports up to 63
     * different channels.
     */
    AV_CHANNEL_ORDER_NATIVE,
    /**
     * The channel order does not correspond to any other predefined order and
     * is stored as an explicit map. For example, this could be used to support
     * layouts with 64 or more channels, or with empty/skipped (AV_CHAN_SILENCE)
     * channels at arbitrary positions.
     */
    AV_CHANNEL_ORDER_CUSTOM,
    /**
     * The audio is represented as the decomposition of the sound field into
     * spherical harmonics. Each channel corresponds to a single expansion
     * component. Channels are ordered according to ACN (Ambisonic Channel
     * Number).
     *
     * The channel with the index n in the stream contains the spherical
     * harmonic of degree l and order m given by
     * @code{.unparsed}
     *   l   = floor(sqrt(n)),
     *   m   = n - l * (l + 1).
     * @endcode
     *
     * Conversely given a spherical harmonic of degree l and order m, the
     * corresponding channel index n is given by
     * @code{.unparsed}
     *   n = l * (l + 1) + m.
     * @endcode
     *
     * Normalization is assumed to be SN3D (Schmidt Semi-Normalization)
     * as defined in AmbiX format $ 2.1.
     */
    AV_CHANNEL_ORDER_AMBISONIC,
};


/**
 * @defgroup channel_masks Audio channel masks
 *
 * A channel layout is a 64-bits integer with a bit set for every channel.
 * The number of bits set must be equal to the number of channels.
 * The value 0 means that the channel layout is not known.
 * @note this data structure is not powerful enough to handle channels
 * combinations that have the same channel multiple times, such as
 * dual-mono.
 *
 * @{
 */
export const AV_CH_FRONT_LEFT =             (1 << AVChannel.AV_CHAN_FRONT_LEFT           )
export const AV_CH_FRONT_RIGHT =            (1 << AVChannel.AV_CHAN_FRONT_RIGHT          )
export const AV_CH_FRONT_CENTER =           (1 << AVChannel.AV_CHAN_FRONT_CENTER         )
export const AV_CH_LOW_FREQUENCY =          (1 << AVChannel.AV_CHAN_LOW_FREQUENCY        )
export const AV_CH_BACK_LEFT =              (1 << AVChannel.AV_CHAN_BACK_LEFT            )
export const AV_CH_BACK_RIGHT =             (1 << AVChannel.AV_CHAN_BACK_RIGHT           )
export const AV_CH_FRONT_LEFT_OF_CENTER =   (1 << AVChannel.AV_CHAN_FRONT_LEFT_OF_CENTER )
export const AV_CH_FRONT_RIGHT_OF_CENTER =  (1 << AVChannel.AV_CHAN_FRONT_RIGHT_OF_CENTER)
export const AV_CH_BACK_CENTER =            (1 << AVChannel.AV_CHAN_BACK_CENTER          )
export const AV_CH_SIDE_LEFT =              (1 << AVChannel.AV_CHAN_SIDE_LEFT            )
export const AV_CH_SIDE_RIGHT =             (1 << AVChannel.AV_CHAN_SIDE_RIGHT           )
export const AV_CH_TOP_CENTER =             (1 << AVChannel.AV_CHAN_TOP_CENTER           )
export const AV_CH_TOP_FRONT_LEFT =         (1 << AVChannel.AV_CHAN_TOP_FRONT_LEFT       )
export const AV_CH_TOP_FRONT_CENTER =       (1 << AVChannel.AV_CHAN_TOP_FRONT_CENTER     )
export const AV_CH_TOP_FRONT_RIGHT =        (1 << AVChannel.AV_CHAN_TOP_FRONT_RIGHT      )
export const AV_CH_TOP_BACK_LEFT =          (1 << AVChannel.AV_CHAN_TOP_BACK_LEFT        )
export const AV_CH_TOP_BACK_CENTER =        (1 << AVChannel.AV_CHAN_TOP_BACK_CENTER      )
export const AV_CH_TOP_BACK_RIGHT =         (1 << AVChannel.AV_CHAN_TOP_BACK_RIGHT       )
export const AV_CH_STEREO_LEFT =            (1 << AVChannel.AV_CHAN_STEREO_LEFT          )
export const AV_CH_STEREO_RIGHT =           (1 << AVChannel.AV_CHAN_STEREO_RIGHT         )
export const AV_CH_WIDE_LEFT =              (1 << AVChannel.AV_CHAN_WIDE_LEFT            )
export const AV_CH_WIDE_RIGHT =             (1 << AVChannel.AV_CHAN_WIDE_RIGHT           )
export const AV_CH_SURROUND_DIRECT_LEFT =   (1 << AVChannel.AV_CHAN_SURROUND_DIRECT_LEFT )
export const AV_CH_SURROUND_DIRECT_RIGHT =  (1 << AVChannel.AV_CHAN_SURROUND_DIRECT_RIGHT)
export const AV_CH_LOW_FREQUENCY_2 =        (1 << AVChannel.AV_CHAN_LOW_FREQUENCY_2      )
export const AV_CH_TOP_SIDE_LEFT =          (1 << AVChannel.AV_CHAN_TOP_SIDE_LEFT        )
export const AV_CH_TOP_SIDE_RIGHT =         (1 << AVChannel.AV_CHAN_TOP_SIDE_RIGHT       )
export const AV_CH_BOTTOM_FRONT_CENTER =    (1 << AVChannel.AV_CHAN_BOTTOM_FRONT_CENTER  )
export const AV_CH_BOTTOM_FRONT_LEFT =      (1 << AVChannel.AV_CHAN_BOTTOM_FRONT_LEFT    )
export const AV_CH_BOTTOM_FRONT_RIGHT =     (1 << AVChannel.AV_CHAN_BOTTOM_FRONT_RIGHT   )

/**
 * @}
 * @defgroup channel_mask_c Audio channel layouts
 * @{
 * */
export const AV_CH_LAYOUT_MONO =              (AV_CH_FRONT_CENTER)
export const AV_CH_LAYOUT_STEREO =            (AV_CH_FRONT_LEFT|AV_CH_FRONT_RIGHT)
export const AV_CH_LAYOUT_2POINT1 =           (AV_CH_LAYOUT_STEREO|AV_CH_LOW_FREQUENCY)
export const AV_CH_LAYOUT_2_1 =               (AV_CH_LAYOUT_STEREO|AV_CH_BACK_CENTER)
export const AV_CH_LAYOUT_SURROUND =          (AV_CH_LAYOUT_STEREO|AV_CH_FRONT_CENTER)
export const AV_CH_LAYOUT_3POINT1 =           (AV_CH_LAYOUT_SURROUND|AV_CH_LOW_FREQUENCY)
export const AV_CH_LAYOUT_4POINT0 =           (AV_CH_LAYOUT_SURROUND|AV_CH_BACK_CENTER)
export const AV_CH_LAYOUT_4POINT1 =           (AV_CH_LAYOUT_4POINT0|AV_CH_LOW_FREQUENCY)
export const AV_CH_LAYOUT_2_2 =               (AV_CH_LAYOUT_STEREO|AV_CH_SIDE_LEFT|AV_CH_SIDE_RIGHT)
export const AV_CH_LAYOUT_QUAD =              (AV_CH_LAYOUT_STEREO|AV_CH_BACK_LEFT|AV_CH_BACK_RIGHT)
export const AV_CH_LAYOUT_5POINT0 =           (AV_CH_LAYOUT_SURROUND|AV_CH_SIDE_LEFT|AV_CH_SIDE_RIGHT)
export const AV_CH_LAYOUT_5POINT1 =           (AV_CH_LAYOUT_5POINT0|AV_CH_LOW_FREQUENCY)
export const AV_CH_LAYOUT_5POINT0_BACK =      (AV_CH_LAYOUT_SURROUND|AV_CH_BACK_LEFT|AV_CH_BACK_RIGHT)
export const AV_CH_LAYOUT_5POINT1_BACK =      (AV_CH_LAYOUT_5POINT0_BACK|AV_CH_LOW_FREQUENCY)
export const AV_CH_LAYOUT_6POINT0 =           (AV_CH_LAYOUT_5POINT0|AV_CH_BACK_CENTER)
export const AV_CH_LAYOUT_6POINT0_FRONT =     (AV_CH_LAYOUT_2_2|AV_CH_FRONT_LEFT_OF_CENTER|AV_CH_FRONT_RIGHT_OF_CENTER)
export const AV_CH_LAYOUT_HEXAGONAL =         (AV_CH_LAYOUT_5POINT0_BACK|AV_CH_BACK_CENTER)
export const AV_CH_LAYOUT_6POINT1 =           (AV_CH_LAYOUT_5POINT1|AV_CH_BACK_CENTER)
export const AV_CH_LAYOUT_6POINT1_BACK =      (AV_CH_LAYOUT_5POINT1_BACK|AV_CH_BACK_CENTER)
export const AV_CH_LAYOUT_6POINT1_FRONT =     (AV_CH_LAYOUT_6POINT0_FRONT|AV_CH_LOW_FREQUENCY)
export const AV_CH_LAYOUT_7POINT0 =           (AV_CH_LAYOUT_5POINT0|AV_CH_BACK_LEFT|AV_CH_BACK_RIGHT)
export const AV_CH_LAYOUT_7POINT0_FRONT =     (AV_CH_LAYOUT_5POINT0|AV_CH_FRONT_LEFT_OF_CENTER|AV_CH_FRONT_RIGHT_OF_CENTER)
export const AV_CH_LAYOUT_7POINT1 =           (AV_CH_LAYOUT_5POINT1|AV_CH_BACK_LEFT|AV_CH_BACK_RIGHT)
export const AV_CH_LAYOUT_7POINT1_WIDE =      (AV_CH_LAYOUT_5POINT1|AV_CH_FRONT_LEFT_OF_CENTER|AV_CH_FRONT_RIGHT_OF_CENTER)
export const AV_CH_LAYOUT_7POINT1_WIDE_BACK = (AV_CH_LAYOUT_5POINT1_BACK|AV_CH_FRONT_LEFT_OF_CENTER|AV_CH_FRONT_RIGHT_OF_CENTER)
export const AV_CH_LAYOUT_OCTAGONAL =         (AV_CH_LAYOUT_5POINT0|AV_CH_BACK_LEFT|AV_CH_BACK_CENTER|AV_CH_BACK_RIGHT)
export const AV_CH_LAYOUT_HEXADECAGONAL =     (AV_CH_LAYOUT_OCTAGONAL|AV_CH_WIDE_LEFT|AV_CH_WIDE_RIGHT|AV_CH_TOP_BACK_LEFT|AV_CH_TOP_BACK_RIGHT|AV_CH_TOP_BACK_CENTER|AV_CH_TOP_FRONT_CENTER|AV_CH_TOP_FRONT_LEFT|AV_CH_TOP_FRONT_RIGHT)
export const AV_CH_LAYOUT_STEREO_DOWNMIX =    (AV_CH_STEREO_LEFT|AV_CH_STEREO_RIGHT)
export const AV_CH_LAYOUT_22POINT2 =          (AV_CH_LAYOUT_5POINT1_BACK|AV_CH_FRONT_LEFT_OF_CENTER|AV_CH_FRONT_RIGHT_OF_CENTER|AV_CH_BACK_CENTER|AV_CH_LOW_FREQUENCY_2|AV_CH_SIDE_LEFT|AV_CH_SIDE_RIGHT|AV_CH_TOP_FRONT_LEFT|AV_CH_TOP_FRONT_RIGHT|AV_CH_TOP_FRONT_CENTER|AV_CH_TOP_CENTER|AV_CH_TOP_BACK_LEFT|AV_CH_TOP_BACK_RIGHT|AV_CH_TOP_SIDE_LEFT|AV_CH_TOP_SIDE_RIGHT|AV_CH_TOP_BACK_CENTER|AV_CH_BOTTOM_FRONT_CENTER|AV_CH_BOTTOM_FRONT_LEFT|AV_CH_BOTTOM_FRONT_RIGHT)

enum AVMatrixEncoding {
    AV_MATRIX_ENCODING_NONE,
    AV_MATRIX_ENCODING_DOLBY,
    AV_MATRIX_ENCODING_DPLII,
    AV_MATRIX_ENCODING_DPLIIX,
    AV_MATRIX_ENCODING_DPLIIZ,
    AV_MATRIX_ENCODING_DOLBYEX,
    AV_MATRIX_ENCODING_DOLBYHEADPHONE,
    AV_MATRIX_ENCODING_NB
};

/**
 * @}
 */

/**
 * An AVChannelCustom defines a single channel within a custom order layout
 *
 * Unlike most structures in FFmpeg, sizeof(AVChannelCustom) is a part of the
 * public ABI.
 *
 * No new fields may be added to it without a major version bump.
 */
export interface AVChannelCustom {
    id: AVChannel;
    /**
     * @note Length is 16
     */
    name: string;
    opaque: OpaquePtr;
}

/**
 * An AVChannelLayout holds information about the channel layout of audio data.
 *
 * A channel layout here is defined as a set of channels ordered in a specific
 * way (unless the channel order is AV_CHANNEL_ORDER_UNSPEC, in which case an
 * AVChannelLayout carries only the channel count).
 *
 * Unlike most structures in Libav, sizeof(AVChannelLayout) is a part of the
 * public ABI and may be used by the caller. E.g. it may be allocated on stack
 * or embedded in caller-defined structs.
 *
 * AVChannelLayout can be initialized as follows:
 * - default initialization with {0}, followed by setting all used fields
 *   correctly;
 * - by assigning one of the predefined AV_CHANNEL_LAYOUT_* initializers;
 * - with a constructor function, such as av_channel_layout_default(),
 *   av_channel_layout_from_mask() or av_channel_layout_from_string().
 *
 * The channel layout must be unitialized with av_channel_layout_uninit()
 *
 * Copying an AVChannelLayout via assigning is forbidden,
 * av_channel_layout_copy() must be used instead (and its return value should
 * be checked)
 *
 * No new fields may be added to it without a major version bump, except for
 * new elements of the union fitting in sizeof(uint64_t).
 */
export interface AVChannelLayout {
    /**
     * Channel order used in this layout.
     * This is a mandatory field.
     */
    order: AVChannelOrder;

    /**
     * Number of channels in this layout. Mandatory field.
     */
    nb_channels: number;

    /**
     * Details about which channels are present in this layout.
     * For AV_CHANNEL_ORDER_UNSPEC, this field is undefined and must not be
     * used.
     */

    /**
     * This member must be used for AV_CHANNEL_ORDER_NATIVE, and may be used
     * for AV_CHANNEL_ORDER_AMBISONIC to signal non-diegetic channels.
     * It is a bitmask, where the position of each set bit means that the
     * AVChannel with the corresponding value is present.
     *
     * I.e. when (mask & (1 << AV_CHAN_FOO)) is non-zero, then AV_CHAN_FOO
     * is present in the layout. Otherwise it is not present.
     *
     * @note when a channel layout using a bitmask is constructed or
     * modified manually (i.e.  not using any of the av_channel_layout_*
     * functions), the code doing it must ensure that the number of set bits
     * is equal to nb_channels.
     */
    mask: number;
    /**
     * This member must be used when the channel order is
     * AV_CHANNEL_ORDER_CUSTOM. It is a nb_channels-sized array, with each
     * element signalling the presence of the AVChannel with the
     * corresponding value in map[i].id.
     *
     * I.e. when map[i].id is equal to AV_CHAN_FOO, then AV_CH_FOO is the
     * i-th channel in the audio data.
     *
     * When map[i].id is in the range between AV_CHAN_AMBISONIC_BASE and
     * AV_CHAN_AMBISONIC_END (inclusive), the channel contains an ambisonic
     * component with ACN index (as defined above)
     * n = map[i].id - AV_CHAN_AMBISONIC_BASE.
     *
     * map[i].name may be filled with a 0-terminated string, in which case
     * it will be used for the purpose of identifying the channel with the
     * convenience functions below. Otherise it must be zeroed.
     */
    map: AVChannelCustom[];

    /**
     * For some private data of the user.
     */
    opaque?: OpaquePtr;
}

export const AV_CHANNEL_LAYOUT_MASK = (nb_channels: number, m: number) => (<AVChannelLayout>{ 
    order: AVChannelOrder.AV_CHANNEL_ORDER_NATIVE, 
    nb_channels,
    mask: m, 
    map: null 
});

export const AV_CHANNEL_LAYOUT_MONO =              AV_CHANNEL_LAYOUT_MASK(1,  AV_CH_LAYOUT_MONO)
export const AV_CHANNEL_LAYOUT_STEREO =            AV_CHANNEL_LAYOUT_MASK(2,  AV_CH_LAYOUT_STEREO)
export const AV_CHANNEL_LAYOUT_2POINT1 =           AV_CHANNEL_LAYOUT_MASK(3,  AV_CH_LAYOUT_2POINT1)
export const AV_CHANNEL_LAYOUT_2_1 =               AV_CHANNEL_LAYOUT_MASK(3,  AV_CH_LAYOUT_2_1)
export const AV_CHANNEL_LAYOUT_SURROUND =          AV_CHANNEL_LAYOUT_MASK(3,  AV_CH_LAYOUT_SURROUND)
export const AV_CHANNEL_LAYOUT_3POINT1 =           AV_CHANNEL_LAYOUT_MASK(4,  AV_CH_LAYOUT_3POINT1)
export const AV_CHANNEL_LAYOUT_4POINT0 =           AV_CHANNEL_LAYOUT_MASK(4,  AV_CH_LAYOUT_4POINT0)
export const AV_CHANNEL_LAYOUT_4POINT1 =           AV_CHANNEL_LAYOUT_MASK(5,  AV_CH_LAYOUT_4POINT1)
export const AV_CHANNEL_LAYOUT_2_2 =               AV_CHANNEL_LAYOUT_MASK(4,  AV_CH_LAYOUT_2_2)
export const AV_CHANNEL_LAYOUT_QUAD =              AV_CHANNEL_LAYOUT_MASK(4,  AV_CH_LAYOUT_QUAD)
export const AV_CHANNEL_LAYOUT_5POINT0 =           AV_CHANNEL_LAYOUT_MASK(5,  AV_CH_LAYOUT_5POINT0)
export const AV_CHANNEL_LAYOUT_5POINT1 =           AV_CHANNEL_LAYOUT_MASK(6,  AV_CH_LAYOUT_5POINT1)
export const AV_CHANNEL_LAYOUT_5POINT0_BACK =      AV_CHANNEL_LAYOUT_MASK(5,  AV_CH_LAYOUT_5POINT0_BACK)
export const AV_CHANNEL_LAYOUT_5POINT1_BACK =      AV_CHANNEL_LAYOUT_MASK(6,  AV_CH_LAYOUT_5POINT1_BACK)
export const AV_CHANNEL_LAYOUT_6POINT0 =           AV_CHANNEL_LAYOUT_MASK(6,  AV_CH_LAYOUT_6POINT0)
export const AV_CHANNEL_LAYOUT_6POINT0_FRONT =     AV_CHANNEL_LAYOUT_MASK(6,  AV_CH_LAYOUT_6POINT0_FRONT)
export const AV_CHANNEL_LAYOUT_HEXAGONAL =         AV_CHANNEL_LAYOUT_MASK(6,  AV_CH_LAYOUT_HEXAGONAL)
export const AV_CHANNEL_LAYOUT_6POINT1 =           AV_CHANNEL_LAYOUT_MASK(7,  AV_CH_LAYOUT_6POINT1)
export const AV_CHANNEL_LAYOUT_6POINT1_BACK =      AV_CHANNEL_LAYOUT_MASK(7,  AV_CH_LAYOUT_6POINT1_BACK)
export const AV_CHANNEL_LAYOUT_6POINT1_FRONT =     AV_CHANNEL_LAYOUT_MASK(7,  AV_CH_LAYOUT_6POINT1_FRONT)
export const AV_CHANNEL_LAYOUT_7POINT0 =           AV_CHANNEL_LAYOUT_MASK(7,  AV_CH_LAYOUT_7POINT0)
export const AV_CHANNEL_LAYOUT_7POINT0_FRONT =     AV_CHANNEL_LAYOUT_MASK(7,  AV_CH_LAYOUT_7POINT0_FRONT)
export const AV_CHANNEL_LAYOUT_7POINT1 =           AV_CHANNEL_LAYOUT_MASK(8,  AV_CH_LAYOUT_7POINT1)
export const AV_CHANNEL_LAYOUT_7POINT1_WIDE =      AV_CHANNEL_LAYOUT_MASK(8,  AV_CH_LAYOUT_7POINT1_WIDE)
export const AV_CHANNEL_LAYOUT_7POINT1_WIDE_BACK = AV_CHANNEL_LAYOUT_MASK(8,  AV_CH_LAYOUT_7POINT1_WIDE_BACK)
export const AV_CHANNEL_LAYOUT_OCTAGONAL =         AV_CHANNEL_LAYOUT_MASK(8,  AV_CH_LAYOUT_OCTAGONAL)
export const AV_CHANNEL_LAYOUT_HEXADECAGONAL =     AV_CHANNEL_LAYOUT_MASK(16, AV_CH_LAYOUT_HEXADECAGONAL)
export const AV_CHANNEL_LAYOUT_STEREO_DOWNMIX =    AV_CHANNEL_LAYOUT_MASK(2,  AV_CH_LAYOUT_STEREO_DOWNMIX)
export const AV_CHANNEL_LAYOUT_22POINT2 =          AV_CHANNEL_LAYOUT_MASK(24, AV_CH_LAYOUT_22POINT2)

export const AV_CHANNEL_LAYOUT_AMBISONIC_FIRST_ORDER = <AVChannelLayout>{ 
    order: AVChannelOrder.AV_CHANNEL_ORDER_AMBISONIC, 
    nb_channels: 4, 
    mask: 0
};

export type AVBPrint = OpaquePtr;

/**
 * Get a human readable string in an abbreviated form describing a given channel.
 * This is the inverse function of @ref av_channel_from_string().
 *
 * @param buf pre-allocated buffer where to put the generated string
 * @param buf_size size in bytes of the buffer.
 * @return amount of bytes needed to hold the output string, or a negative AVERROR
 *         on failure. If the returned value is bigger than buf_size, then the
 *         string was truncated.
 */
export function av_channel_name(buf: Ref<string>, buf_size: number, channel: AVChannel): number { throw new NotImplemented(); }

/**
 * bprint variant of av_channel_name().
 *
 * @note the string will be appended to the bprint buffer.
 */
export function av_channel_name_bprint(bp: AVBPrint, channel_id: AVChannel): void { throw new NotImplemented(); }

/**
 * Get a human readable string describing a given channel.
 *
 * @param buf pre-allocated buffer where to put the generated string
 * @param buf_size size in bytes of the buffer.
 * @return amount of bytes needed to hold the output string, or a negative AVERROR
 *         on failure. If the returned value is bigger than buf_size, then the
 *         string was truncated.
 */
export function av_channel_description(buf: Ref<string>, buf_size: number,channel: AVChannel): number { throw new NotImplemented(); }

/**
 * bprint variant of av_channel_description().
 *
 * @note the string will be appended to the bprint buffer.
 */
export function av_channel_description_bprint(bp: AVBPrint,channel_id: AVChannel): void { throw new NotImplemented(); }

/**
 * This is the inverse function of @ref av_channel_name().
 *
 * @return the channel with the given name
 *         AV_CHAN_NONE when name does not identify a known channel
 */
export function av_channel_from_string(name: string): AVChannel { throw new NotImplemented(); };

/**
 * Initialize a native channel layout from a bitmask indicating which channels
 * are present.
 *
 * @param channel_layout the layout structure to be initialized
 * @param mask bitmask describing the channel layout
 *
 * @return 0 on success
 *         AVERROR(EINVAL) for invalid mask values
 */
export function av_channel_layout_from_mask(channel_layout: AVChannelLayout, mask: number): number { throw new NotImplemented(); }

/**
 * Initialize a channel layout from a given string description.
 * The input string can be represented by:
 *  - the formal channel layout name (returned by av_channel_layout_describe())
 *  - single or multiple channel names (returned by av_channel_name(), eg. "FL",
 *    or concatenated with "+", each optionally containing a custom name after
 *    a "@", eg. "FL@Left+FR@Right+LFE")
 *  - a decimal or hexadecimal value of a native channel layout (eg. "4" or "0x4")
 *  - the number of channels with default layout (eg. "4c")
 *  - the number of unordered channels (eg. "4C" or "4 channels")
 *  - the ambisonic order followed by optional non-diegetic channels (eg.
 *    "ambisonic 2+stereo")
 *
 * @param channel_layout input channel layout
 * @param str string describing the channel layout
 * @return 0 channel layout was detected, AVERROR_INVALIDATATA otherwise
 */
export function av_channel_layout_from_string(channel_layout: AVChannelLayout, str: string): number { throw new NotImplemented(); }

/**
 * Get the default channel layout for a given number of channels.
 *
 * @param channel_layout the layout structure to be initialized
 * @param nb_channels number of channels
 */
export function av_channel_layout_default(ch_layout: AVChannelLayout, nb_channels: number): void { throw new NotImplemented(); }

/**
 * Iterate over all standard channel layouts.
 *
 * @param opaque a pointer where libavutil will store the iteration state. Must
 *               point to NULL to start the iteration.
 *
 * @return the standard channel layout or NULL when the iteration is
 *         finished
 */
export function av_channel_layout_standard(opaque: Ref<OpaquePtr>): AVChannelLayout { throw new NotImplemented(); };

/**
 * Free any allocated data in the channel layout and reset the channel
 * count to 0.
 *
 * @param channel_layout the layout structure to be uninitialized
 */
export function av_channel_layout_uninit(channel_layout: AVChannelLayout): void { throw new NotImplemented(); }

/**
 * Make a copy of a channel layout. This differs from just assigning src to dst
 * in that it allocates and copies the map for AV_CHANNEL_ORDER_CUSTOM.
 *
 * @note the destination channel_layout will be always uninitialized before copy.
 *
 * @param dst destination channel layout
 * @param src source channel layout
 * @return 0 on success, a negative AVERROR on error.
 */
export function av_channel_layout_copy(dst: AVChannelLayout, src: AVChannelLayout): number { throw new NotImplemented(); }

/**
 * Get a human-readable string describing the channel layout properties.
 * The string will be in the same format that is accepted by
 * @ref av_channel_layout_from_string(), allowing to rebuild the same
 * channel layout, except for opaque pointers.
 *
 * @param channel_layout channel layout to be described
 * @param buf pre-allocated buffer where to put the generated string
 * @param buf_size size in bytes of the buffer.
 * @return amount of bytes needed to hold the output string, or a negative AVERROR
 *         on failure. If the returned value is bigger than buf_size, then the
 *         string was truncated.
 */
export function av_channel_layout_describe(channel_layout: AVChannelLayout, buf: Ref<string>, buf_size: number): number { throw new NotImplemented(); }

/**
 * bprint variant of av_channel_layout_describe().
 *
 * @note the string will be appended to the bprint buffer.
 * @return 0 on success, or a negative AVERROR value on failure.
 */
export function av_channel_layout_describe_bprint(channel_layout: AVChannelLayout, bp: AVBPrint): number { throw new NotImplemented(); }

/**
 * Get the channel with the given index in a channel layout.
 *
 * @param channel_layout input channel layout
 * @return channel with the index idx in channel_layout on success or
 *         AV_CHAN_NONE on failure (if idx is not valid or the channel order is
 *         unspecified)
 */
export function av_channel_layout_channel_from_index(channel_layout: AVChannelLayout, idx: number): AVChannel { throw new NotImplemented(); };

/**
 * Get the index of a given channel in a channel layout. In case multiple
 * channels are found, only the first match will be returned.
 *
 * @param channel_layout input channel layout
 * @return index of channel in channel_layout on success or a negative number if
 *         channel is not present in channel_layout.
 */
export function av_channel_layout_index_from_channel(channel_layout: AVChannelLayout, channel: AVChannel): number { throw new NotImplemented(); }

/**
 * Get the index in a channel layout of a channel described by the given string.
 * In case multiple channels are found, only the first match will be returned.
 *
 * This function accepts channel names in the same format as
 * @ref av_channel_from_string().
 *
 * @param channel_layout input channel layout
 * @return a channel index described by the given string, or a negative AVERROR
 *         value.
 */
export function av_channel_layout_index_from_string(channel_layout: AVChannelLayout, name: string): number { throw new NotImplemented(); }

/**
 * Get a channel described by the given string.
 *
 * This function accepts channel names in the same format as
 * @ref av_channel_from_string().
 *
 * @param channel_layout input channel layout
 * @return a channel described by the given string in channel_layout on success
 *         or AV_CHAN_NONE on failure (if the string is not valid or the channel
 *         order is unspecified)
 */
export function av_channel_layout_channel_from_string(channel_layout: AVChannelLayout, name: string): AVChannel { throw new NotImplemented(); };

/**
 * Find out what channels from a given set are present in a channel layout,
 * without regard for their positions.
 *
 * @param channel_layout input channel layout
 * @param mask a combination of AV_CH_* representing a set of channels
 * @return a bitfield representing all the channels from mask that are present
 *         in channel_layout
 */
export function av_channel_layout_subset(channel_layout: AVChannelLayout, mask: number): number { throw new NotImplemented(); };

/**
 * Check whether a channel layout is valid, i.e. can possibly describe audio
 * data.
 *
 * @param channel_layout input channel layout
 * @return 1 if channel_layout is valid, 0 otherwise.
 */
export function av_channel_layout_check(channel_layout: AVChannelLayout): number { throw new NotImplemented(); }

/**
 * Check whether two channel layouts are semantically the same, i.e. the same
 * channels are present on the same positions in both.
 *
 * If one of the channel layouts is AV_CHANNEL_ORDER_UNSPEC, while the other is
 * not, they are considered to be unequal. If both are AV_CHANNEL_ORDER_UNSPEC,
 * they are considered equal iff the channel counts are the same in both.
 *
 * @param chl input channel layout
 * @param chl1 input channel layout
 * @return 0 if chl and chl1 are equal, 1 if they are not equal. A negative
 *         AVERROR code if one or both are invalid.
 */
export function av_channel_layout_compare(chl: AVChannelLayout, chl1: AVChannelLayout): number { throw new NotImplemented(); }
