/*
* copyright (c) 2006 Michael Niedermayer <michaelni@gmx.at>
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

import { AVChannel } from "./channel-layout";
import { AVRational } from "./rational";

export declare class AVUtil {
    /**
     * The version number of libavutil (LIBAVUTIL_VERSION_INT constant)
     */
    static readonly version: string;

    /**
     * Return an informative version string. This usually is the actual release
     * version number or a git commit description. This string has no fixed format
     * and can change any time. It should never be parsed by code.
     */
    static readonly versionInfo: string;

    /**
     * Return the libavutil build-time configuration.
     */
    static readonly configuration: string;

    /**
     * Return the libavutil license.
     */
    static readonly license: string;
    
    /**
     * Return the fractional representation of the internal time base.
     */
    static readonly timebase: AVRational;
    
    /**
     * Return a string describing the media_type enum, NULL if media_type
     * is unknown.
     */
    static getMediaTypeString(media_type: AVMediaType): string;

    /**
     * Return a single letter to describe the given picture type
     * pict_type.
     *
     * @param[in] pict_type the picture type @return a single character
     * representing the picture type, '?' if pict_type is unknown
     */
    static getPictureTypeString(pict_type: AVPictureType): string;
    
    /**
     * Fill the provided buffer with a string containing a FourCC (four-character
     * code) representation.
     *
     * @param buf    a buffer with size in bytes of at least AV_FOURCC_MAX_STRING_SIZE
     * @param fourcc the fourcc to represent
     * @return the buffer in input
     */
    static getFourCCString(fourcc: number): string;
    
    /**
     * Get a human readable string in an abbreviated form describing a given channel.
     * This is the inverse function of channelFromName().
     */
    getChannelName(channel: AVChannel): string;

    /**
     * Get a human readable string describing a given channel.
     */
    getChannelDescription(channel: AVChannel): string;

    /**
     * This is the inverse function of @ref av_channel_name().
     *
     * @return the channel with the given name
     *         AV_CHAN_NONE when name does not identify a known channel
     */
    getChannelFromName(name: string): AVChannel;
}

/**
 * Media Type
 */
export enum AVMediaType {
    AVMEDIA_TYPE_UNKNOWN = -1,  ///< Usually treated as AVMEDIA_TYPE_DATA
    AVMEDIA_TYPE_VIDEO,
    AVMEDIA_TYPE_AUDIO,
    AVMEDIA_TYPE_DATA,          ///< Opaque data information usually continuous
    AVMEDIA_TYPE_SUBTITLE,
    AVMEDIA_TYPE_ATTACHMENT,    ///< Opaque data information usually sparse
    AVMEDIA_TYPE_NB
};

export const FF_LAMBDA_SHIFT = 7;
export const FF_LAMBDA_SCALE = (1<<FF_LAMBDA_SHIFT);

/**
 * factor to convert from H.263 QP to lambda
 */
export const FF_QP2LAMBDA = 118; 
export const FF_LAMBDA_MAX = (256*128-1);
export const FF_QUALITY_SCALE = FF_LAMBDA_SCALE;

/**
 * Undefined timestamp value.
 *
 * Usually reported by demuxer that work on containers that do not provide
 * either pts or dts.
 */
export const AV_NOPTS_VALUE = Number(BigInt(0x8000000000000000));

export enum AVPictureType {

    /**
     * Undefined
     */
    AV_PICTURE_TYPE_NONE = 0,

    /**
     * Intra
     */
    AV_PICTURE_TYPE_I,

    /**
     * Predicted
     */
    AV_PICTURE_TYPE_P,

    /**
     * Bi-dir predicted
     */
    AV_PICTURE_TYPE_B,

    /**
     * S(GMC)-VOP MPEG-4
     */
    AV_PICTURE_TYPE_S,

    /**
     * Switching Intra
     */
    AV_PICTURE_TYPE_SI,

    /**
     * Switching Predicted
     */
    AV_PICTURE_TYPE_SP,

    /**
     * BI type
     */
    AV_PICTURE_TYPE_BI,
};

export const AV_FOURCC_MAX_STRING_SIZE = 32;
