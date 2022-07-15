/*
 * Codec descriptors public API
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

import { AVMediaType } from "../avutil";
import { NotImplemented } from "../helpers";
import { AVProfile } from "./codec";
import { AVCodecID } from "./codec_id";

/**
 * @addtogroup lavc_core
 * @{
 */

/**
 * This struct describes the properties of a single codec described by an
 * AVCodecID.
 * @see avcodec_descriptor_get()
 */
export interface AVCodecDescriptor {
    id: AVCodecID;
    type: AVMediaType;
    /**
     * Name of the codec described by this descriptor. It is non-empty and
     * unique for each codec descriptor. It should contain alphanumeric
     * characters and '_' only.
     */
    name: string;
    /**
     * A more descriptive name for this codec. May be NULL.
     */
    long_name: string;
    /**
     * Codec properties, a combination of AV_CODEC_PROP_* flags.
     */
    props: number;
    /**
     * MIME type(s) associated with the codec.
     * May be NULL; if not, a NULL-terminated array of MIME types.
     * The first item is always non-NULL and is the preferred MIME type.
     */
    mime_types: string[];
    /**
     * If non-NULL, an array of profiles recognized for this codec.
     * Terminated with FF_PROFILE_UNKNOWN.
     */
    profiles: AVProfile[];
}

/**
 * Codec uses only intra compression.
 * Video and audio codecs only.
 */
export const AV_CODEC_PROP_INTRA_ONLY =    (1 << 0);
/**
 * Codec supports lossy compression. Audio and video codecs only.
 * @note a codec may support both lossy and lossless
 * compression modes
 */
export const AV_CODEC_PROP_LOSSY =         (1 << 1);
/**
 * Codec supports lossless compression. Audio and video codecs only.
 */
export const AV_CODEC_PROP_LOSSLESS =      (1 << 2);
/**
 * Codec supports frame reordering. That is, the coded order (the order in which
 * the encoded packets are output by the encoders / stored / input to the
 * decoders) may be different from the presentation order of the corresponding
 * frames.
 *
 * For codecs that do not have this property set, PTS and DTS should always be
 * equal.
 */
export const AV_CODEC_PROP_REORDER =       (1 << 3);
/**
 * Subtitle codec is bitmap based
 * Decoded AVSubtitle data can be read from the AVSubtitleRect->pict field.
 */
export const AV_CODEC_PROP_BITMAP_SUB =    (1 << 16);
/**
 * Subtitle codec is text based.
 * Decoded AVSubtitle data can be read from the AVSubtitleRect->ass field.
 */
export const AV_CODEC_PROP_TEXT_SUB =      (1 << 17);

/**
 * @return descriptor for given codec ID or NULL if no descriptor exists.
 */
export function avcodec_descriptor_get(id: AVCodecID): AVCodecDescriptor { throw new NotImplemented(); }

/**
 * Iterate over all codec descriptors known to libavcodec.
 *
 * @param prev previous descriptor. NULL to get the first descriptor.
 *
 * @return next descriptor or NULL after the last descriptor
 */
export function avcodec_descriptor_next(prev: AVCodecDescriptor): AVCodecDescriptor { throw new NotImplemented(); }

/**
 * @return codec descriptor with the given name or NULL if no such descriptor
 *         exists.
 */
export function avcodec_descriptor_get_by_name(name: string): AVCodecDescriptor { throw new NotImplemented(); }

/**
 * @}
 */