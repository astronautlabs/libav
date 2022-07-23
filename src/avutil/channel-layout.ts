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

export const AV_CH_FRONT_LEFT =             0x00000001;
export const AV_CH_FRONT_RIGHT =            0x00000002;
export const AV_CH_FRONT_CENTER =           0x00000004;
export const AV_CH_LOW_FREQUENCY =          0x00000008;
export const AV_CH_BACK_LEFT =              0x00000010;
export const AV_CH_BACK_RIGHT =             0x00000020;
export const AV_CH_FRONT_LEFT_OF_CENTER =   0x00000040;
export const AV_CH_FRONT_RIGHT_OF_CENTER =  0x00000080;
export const AV_CH_BACK_CENTER =            0x00000100;
export const AV_CH_SIDE_LEFT =              0x00000200;
export const AV_CH_SIDE_RIGHT =             0x00000400;
export const AV_CH_TOP_CENTER =             0x00000800;
export const AV_CH_TOP_FRONT_LEFT =         0x00001000;
export const AV_CH_TOP_FRONT_CENTER =       0x00002000;
export const AV_CH_TOP_FRONT_RIGHT =        0x00004000;
export const AV_CH_TOP_BACK_LEFT =          0x00008000;
export const AV_CH_TOP_BACK_CENTER =        0x00010000;
export const AV_CH_TOP_BACK_RIGHT =         0x00020000;
export const AV_CH_STEREO_LEFT =            0x20000000;  ///< Stereo downmix.
export const AV_CH_STEREO_RIGHT =           0x40000000;  ///< See AV_CH_STEREO_LEFT.
export const AV_CH_WIDE_LEFT =              0x0000000080000000;
export const AV_CH_WIDE_RIGHT =             0x0000000100000000;
export const AV_CH_SURROUND_DIRECT_LEFT =   0x0000000200000000;
export const AV_CH_SURROUND_DIRECT_RIGHT =  0x0000000400000000;
export const AV_CH_LOW_FREQUENCY_2 =        0x0000000800000000;
export const AV_CH_TOP_SIDE_LEFT =          0x0000001000000000;
export const AV_CH_TOP_SIDE_RIGHT =         0x0000002000000000;
export const AV_CH_BOTTOM_FRONT_CENTER =    0x0000004000000000;
export const AV_CH_BOTTOM_FRONT_LEFT =      0x0000008000000000;
export const AV_CH_BOTTOM_FRONT_RIGHT =     0x0000010000000000;

/** Channel mask value used for AVCodecContext.request_channel_layout
    to indicate that the user requests the channel order of the decoder output
    to be the native codec channel order. */
export const AV_CH_LAYOUT_NATIVE =          0x8000000000000000;

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
