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
 * Public dictionary API.
 * @deprecated
 *  AVDictionary is provided for compatibility with libav. It is both in
 *  implementation as well as API inefficient. It does not scale and is
 *  extremely slow with large dictionaries.
 *  It is recommended that new code uses our tree container from tree.c/h
 *  where applicable, which uses AVL trees to achieve O(log n) performance.
 */

 import { NotImplemented, OpaquePtr, Out, Ref } from "../helpers";

/**
 * @addtogroup lavu_dict AVDictionary
 * @ingroup lavu_data
 *
 * @brief Simple key:value store
 *
 * @{
 * Dictionaries are used for storing key:value pairs. To create
 * an AVDictionary, simply pass an address of a NULL pointer to
 * av_dict_set(). NULL can be used as an empty dictionary wherever
 * a pointer to an AVDictionary is required.
 * Use av_dict_get() to retrieve an entry or iterate over all
 * entries and finally av_dict_free() to free the dictionary
 * and all its contents.
 *
 @code
    AVDictionary *d = NULL;           // "create" an empty dictionary
    AVDictionaryEntry *t = NULL;

    av_dict_set(&d, "foo", "bar", 0); // add an entry

    char *k = av_strdup("key");       // if your strings are already allocated,
    char *v = av_strdup("value");     // you can avoid copying them like this
    av_dict_set(&d, k, v, AV_DICT_DONT_STRDUP_KEY | AV_DICT_DONT_STRDUP_VAL);

    while (t = av_dict_get(d, "", t, AV_DICT_IGNORE_SUFFIX)) {
        <....>                             // iterate over all entries in d
    }
    av_dict_free(&d);
@endcode
*/

/**< Only get an entry with exact-case key match. Only relevant in av_dict_get(). */
export const AV_DICT_MATCH_CASE =      1;
/**< Return first entry in a dictionary whose first part corresponds to the search key,
    ignoring the suffix of the found key string. Only relevant in av_dict_get(). */
export const AV_DICT_IGNORE_SUFFIX =   2;
/**< Take ownership of a key that's been
    allocated with av_malloc() or another memory allocation function. */
export const AV_DICT_DONT_STRDUP_KEY = 4;
/**< Take ownership of a value that's been
    allocated with av_malloc() or another memory allocation function. */
export const AV_DICT_DONT_STRDUP_VAL = 8;
///< Don't overwrite existing entries.
export const AV_DICT_DONT_OVERWRITE = 16;
/**< If the entry already exists, append to it.  Note that no
    delimiter is added, the strings are simply concatenated. */
export const AV_DICT_APPEND =         32;
/**< Allow to store several equal keys in the dictionary */
export const AV_DICT_MULTIKEY =       64;

export declare class AVDictionaryEntry {
    key: string;
    value: string;
};

export declare class AVDictionary {
    constructor();

    /**
     * Get a dictionary entry with matching key.
     *
     * To iterate through all the dictionary entries, you can set the matching key
     * to the null string "" and set the AV_DICT_IGNORE_SUFFIX flag.
     *
     * @param prev Set to the previous matching element to find the next.
     *             If set to NULL the first matching element is returned.
     * @param key matching key
     * @param flags a collection of AV_DICT_* flags controlling how the entry is retrieved
     * @return found entry or undefined in case no matching entry was found in the dictionary
     */
    get(key: string, prev?: AVDictionaryEntry, flags?: number): AVDictionaryEntry;

    
    /**
     * Set the given key, overwriting an existing entry.
     *
     * Warning: Adding a new entry to a dictionary invalidates all existing entries
     * previously returned with av_dict_get.
     *
     * @param key key to add to the dictionary
     * @param value value to add. Passing null will cause an existing entry to be deleted.
     * @return True on success
     */
    set(key: string, value: string, flags?: number): boolean;

    /**
     * Copy entries from one AVDictionary into another.
     * @param flags flags to use when setting entries in the destination dictionary
     * @note metadata is read using the AV_DICT_IGNORE_SUFFIX flag
     */
    copy(destination: AVDictionary, flags?: number);

    /**
     * Clear all keys
     */
    clear();
    readonly count: number;
    readonly keys: string[];
}
