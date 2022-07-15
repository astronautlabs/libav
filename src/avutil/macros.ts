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

/**
 * @file
 * @ingroup lavu
 * Utility Preprocessor macros
 */

import * as os from 'os';
export function AV_NE(be: any, le: any) {
    return os.endianness() === 'BE' ? be : le;
}

/**
 * Comparator.
 * For two numerical expressions x and y, gives 1 if x > y, -1 if x < y, and 0
 * if x == y. This is useful for instance in a qsort comparator callback.
 * Furthermore, compilers are able to optimize this to branchless code, and
 * there is no risk of overflow with signed types.
 * As with many macros, this evaluates its argument multiple times, it thus
 * must not have a side-effect.
 */
export const FFDIFFSIGN = (x: number, y: number) => x > y ? 1 : x === y ? 0 : -1;

export const FFMAX = (a: number, b: number) => a > b ? a : b;
export const FFMAX3 = (a: number, b: number, c: number) => FFMAX(FFMAX(a, b), c);
export const FFMIN = (a: number, b: number) => a > b ? b : a;
export const FFMIN3 = (a: number, b: number, c: number) => FFMIN(FFMIN(a, b), c);

export const MKTAGn = (a: number, b: number, c: number, d: number) => (a | (b << 8) | (c << 16) | (d << 24));
export const MKBETAGn = (a: number, b: number, c: number, d: number) => (d | (c << 8) | (b << 16) | (a << 24));

export const ASCII_CHAR = (str: string): number => {
    return Buffer.from(str)[0];
}

export const MKTAG = (a: string, b: string, c: string, d: string) => MKTAGn(ASCII_CHAR(a), ASCII_CHAR(b), ASCII_CHAR(c), ASCII_CHAR(d));
export const MKBETAG = (a: string, b: string, c: string, d: string) => MKBETAGn(ASCII_CHAR(a), ASCII_CHAR(b), ASCII_CHAR(c), ASCII_CHAR(d));
export const FFALIGN = (x: number, a: number) => ((x + a - 1) & ~( a - 1));