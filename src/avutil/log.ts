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

import { NotImplemented, OpaquePtr, Out, Ref } from "../helpers";
import { AVOption, AVOptionRanges } from "./opt";

export enum AVClassCategory {
    AV_CLASS_CATEGORY_NA = 0,
    AV_CLASS_CATEGORY_INPUT,
    AV_CLASS_CATEGORY_OUTPUT,
    AV_CLASS_CATEGORY_MUXER,
    AV_CLASS_CATEGORY_DEMUXER,
    AV_CLASS_CATEGORY_ENCODER,
    AV_CLASS_CATEGORY_DECODER,
    AV_CLASS_CATEGORY_FILTER,
    AV_CLASS_CATEGORY_BITSTREAM_FILTER,
    AV_CLASS_CATEGORY_SWSCALER,
    AV_CLASS_CATEGORY_SWRESAMPLER,
    AV_CLASS_CATEGORY_DEVICE_VIDEO_OUTPUT = 40,
    AV_CLASS_CATEGORY_DEVICE_VIDEO_INPUT,
    AV_CLASS_CATEGORY_DEVICE_AUDIO_OUTPUT,
    AV_CLASS_CATEGORY_DEVICE_AUDIO_INPUT,
    AV_CLASS_CATEGORY_DEVICE_OUTPUT,
    AV_CLASS_CATEGORY_DEVICE_INPUT,
    AV_CLASS_CATEGORY_NB  ///< not part of ABI/API
}

export function AV_IS_INPUT_DEVICE(category: AVClassCategory) {
    return [
            AVClassCategory.AV_CLASS_CATEGORY_DEVICE_VIDEO_INPUT,
            AVClassCategory.AV_CLASS_CATEGORY_DEVICE_AUDIO_INPUT,
            AVClassCategory.AV_CLASS_CATEGORY_DEVICE_INPUT
        ].includes(category);
}

export function AV_IS_OUTPUT_DEVICE(category: AVClassCategory) {
    return [
            AVClassCategory.AV_CLASS_CATEGORY_DEVICE_VIDEO_OUTPUT,
            AVClassCategory.AV_CLASS_CATEGORY_DEVICE_AUDIO_OUTPUT,
            AVClassCategory.AV_CLASS_CATEGORY_DEVICE_OUTPUT
        ].includes(category);
}

/**
 * Describe the class of an AVClass context structure. That is an
 * arbitrary struct of which the first field is a pointer to an
 * AVClass struct (e.g. AVCodecContext, AVFormatContext etc.).
 */
export interface AVClass {
    /**
     * The name of the class; usually it is the same name as the
     * context structure type to which the AVClass is associated.
     */
    class_name: string;

    /**
     * A pointer to a function which returns the name of a context
     * instance ctx associated with the class.
     */
    item_name: (ctx: OpaquePtr) => string;

    /**
     * a pointer to the first option specified in the class if any or NULL
     *
     * @see av_set_default_options()
     */
    option: AVOption;

    /**
     * LIBAVUTIL_VERSION with which this structure was created.
     * This is used to allow fields to be added without requiring major
     * version bumps everywhere.
     */

    version: number;

    /**
     * Offset in the structure where log_level_offset is stored.
     * 0 means there is no such variable
     */
    log_level_offset_offset: number;

    /**
     * Offset in the structure where a pointer to the parent context for
     * logging is stored. For example a decoder could pass its AVCodecContext
     * to eval as such a parent context, which an av_log() implementation
     * could then leverage to display the parent context.
     * The offset can be NULL.
     */
    parent_log_context_offset: number;

    /**
     * Category used for visualization (like color)
     * This is only set if the category is equal for all objects using this class.
     * available since version (51 << 16 | 56 << 8 | 100)
     */
    category: AVClassCategory;

    /**
     * Callback to return the category.
     * available since version (51 << 16 | 59 << 8 | 100)
     */
    get_category: (ctx: OpaquePtr) => AVClassCategory;

    /**
     * Callback to return the supported/allowed ranges.
     * available since version (52.12)
     */
    query_ranges: (ranges: Out<AVOptionRanges>, obj: OpaquePtr, key: string, flags: number) => number;

    /**
     * Return next AVOptions-enabled child or NULL
     */
    child_next: (obj: OpaquePtr, prev: OpaquePtr) => OpaquePtr;

    /**
     * Iterate over the AVClasses corresponding to potential AVOptions-enabled
     * children.
     *
     * @param iter pointer to opaque iteration state. The caller must initialize
     *             *iter to NULL before the first call.
     * @return AVClass for the next AVOptions-enabled child or NULL if there are
     *         no more such children.
     *
     * @note The difference between child_next and this is that child_next
     *       iterates over _already existing_ objects, while child_class_iterate
     *       iterates over _all possible_ children.
     */
    child_class_iterate: (iter: Ref<OpaquePtr>) => AVClass;
}

/**
 * @addtogroup lavu_log
 *
 * @{
 *
 * @defgroup lavu_log_constants Logging Constants
 *
 * @{
 */

/**
 * Print no output.
 */
export const AV_LOG_QUIET =    -8;

/**
 * Something went really wrong and we will crash now.
 */
export const AV_LOG_PANIC =     0;

/**
 * Something went wrong and recovery is not possible.
 * For example, no header was found for a format which depends
 * on headers or an illegal combination of parameters is used.
 */
export const AV_LOG_FATAL =     8;

/**
 * Something went wrong and cannot losslessly be recovered.
 * However, not all future data is affected.
 */
export const AV_LOG_ERROR =    16;

/**
 * Something somehow does not look correct. This may or may not
 * lead to problems. An example would be the use of '-vstrict -2'.
 */
export const AV_LOG_WARNING =  24;

/**
 * Standard information.
 */
export const AV_LOG_INFO =     32;

/**
 * Detailed information.
 */
export const AV_LOG_VERBOSE =  40;

/**
 * Stuff which is only useful for libav* developers.
 */
export const AV_LOG_DEBUG =    48;

/**
 * Extremely verbose debugging, useful for libav* development.
 */
export const AV_LOG_TRACE =    56;

export const AV_LOG_MAX_OFFSET = (AV_LOG_TRACE - AV_LOG_QUIET);

/**
 * @}
 */

/**
 * Sets additional colors for extended debugging sessions.
 * ```
 * av_log(ctx, AV_LOG_DEBUG|AV_LOG_C(134), "Message in purple\n");
 * ```
 * Requires 256color terminal support. Uses outside debugging is not
 * recommended.
 */
export const AV_LOG_C = (x: number) => x << 8;

/**
 * Send the specified message to the log if the level is less than or equal
 * to the current av_log_level. By default, all logging messages are sent to
 * stderr. This behavior can be altered by setting a different logging callback
 * function.
 * @see av_log_set_callback
 *
 * @param avcl A pointer to an arbitrary struct of which the first field is a
 *        pointer to an AVClass struct or NULL if general log.
 * @param level The importance level of the message expressed using a @ref
 *        lavu_log_constants "Logging Constant".
 * @param fmt The format string (printf-compatible) that specifies how
 *        subsequent arguments are converted to output.
 */
export function av_log(avcl: OpaquePtr, level: number, fmt: string, ...args) {
    // @nodeTODO av_printf_format(3, 4):
};

/**
 * Send the specified message to the log once with the initial_level and then with
 * the subsequent_level. By default, all logging messages are sent to
 * stderr. This behavior can be altered by setting a different logging callback
 * function.
 * @see av_log
 *
 * @param avcl A pointer to an arbitrary struct of which the first field is a
 *        pointer to an AVClass struct or NULL if general log.
 * @param initial_level importance level of the message expressed using a @ref
 *        lavu_log_constants "Logging Constant" for the first occurance.
 * @param subsequent_level importance level of the message expressed using a @ref
 *        lavu_log_constants "Logging Constant" after the first occurance.
 * @param fmt The format string (printf-compatible) that specifies how
 *        subsequent arguments are converted to output.
 * @param state a variable to keep trak of if a message has already been printed
 *        this must be initialized to 0 before the first use. The same state
 *        must not be accessed by 2 Threads simultaneously.
 */
export function av_log_once(avcl: OpaquePtr, initial_level: number, subsequent_level: number, state: Ref<number>, fmt: string, ...args) {
    // @nodeTODO av_printf_format(5, 6)
}


/**
 * Send the specified message to the log if the level is less than or equal
 * to the current av_log_level. By default, all logging messages are sent to
 * stderr. This behavior can be altered by setting a different logging callback
 * function.
 * @see av_log_set_callback
 *
 * @param avcl A pointer to an arbitrary struct of which the first field is a
 *        pointer to an AVClass struct.
 * @param level The importance level of the message expressed using a @ref
 *        lavu_log_constants "Logging Constant".
 * @param fmt The format string (printf-compatible) that specifies how
 *        subsequent arguments are converted to output.
 * @param vl The arguments referenced by the format string.
 */
export function av_vlog(avcl: OpaquePtr, level: number, fmt: string, vl: any[]) { throw new NotImplemented(); }

/**
 * Get the current log level
 *
 * @see lavu_log_constants
 *
 * @return Current log level
 */
export function av_log_get_level(): number { throw new NotImplemented(); }

/**
 * Set the log level
 *
 * @see lavu_log_constants
 *
 * @param level Logging level
 */
export function av_log_set_level(level: number) { throw new NotImplemented(); }

/**
 * Set the logging callback
 *
 * @note The callback must be thread safe, even if the application does not use
 *       threads itself as some codecs are multithreaded.
 *
 * @see av_log_default_callback
 *
 * @param callback A logging function with a compatible signature.
 */
export function av_log_set_callback(callback: (avcl: OpaquePtr, level: number, fmt: string, args: any[]) => void) { throw new NotImplemented(); }

/**
 * Default logging callback
 *
 * It prints the message to stderr, optionally colorizing it.
 *
 * @param avcl A pointer to an arbitrary struct of which the first field is a
 *        pointer to an AVClass struct.
 * @param level The importance level of the message expressed using a @ref
 *        lavu_log_constants "Logging Constant".
 * @param fmt The format string (printf-compatible) that specifies how
 *        subsequent arguments are converted to output.
 * @param vl The arguments referenced by the format string.
 */
export function av_log_default_callback(avcl: OpaquePtr, level: number, fmt: string, ...vl) { throw new NotImplemented(); }

/**
 * Return the context name
 *
 * @param  ctx The AVClass context
 *
 * @return The AVClass class_name
 */
export function av_default_item_name(ctx: OpaquePtr): string { throw new NotImplemented(); }
export function av_default_get_category(ptr: OpaquePtr): AVClassCategory { throw new NotImplemented(); }

/**
 * Format a line of log the same way as the default callback.
 * @param line          buffer to receive the formatted line
 * @param line_size     size of the buffer
 * @param print_prefix  used to store whether the prefix must be printed;
 *                      must point to a persistent integer initially set to 1
 */
export function av_log_format_line(ptr: OpaquePtr, level: number, fmt: string, args: any[],
                        line: string, line_size: number, print_prefix: Ref<number>) { throw new NotImplemented(); }

/**
 * Format a line of log the same way as the default callback.
 * @param line          buffer to receive the formatted line;
 *                      may be NULL if line_size is 0
 * @param line_size     size of the buffer; at most line_size-1 characters will
 *                      be written to the buffer, plus one null terminator
 * @param print_prefix  used to store whether the prefix must be printed;
 *                      must point to a persistent integer initially set to 1
 * @return Returns a negative value if an error occurred, otherwise returns
 *         the number of characters that would have been written for a
 *         sufficiently large buffer, not including the terminating null
 *         character. If the return value is not less than line_size, it means
 *         that the log message was truncated to fit the buffer.
 */
export function av_log_format_line2(ptr: OpaquePtr, level: number, fmt: string, args,
                        line: string, line_size: number, print_prefix: Ref<number>): number { throw new NotImplemented(); }

/**
 * Skip repeated messages, this requires the user app to use av_log() instead of
 * (f)printf as the 2 would otherwise interfere and lead to
 * "Last message repeated x times" messages below (f)printf messages with some
 * bad luck.
 * Also to receive the last, "last repeated" line if any, the user app must
 * call av_log(NULL, AV_LOG_QUIET, "%s", ""); at the end
 */
export const AV_LOG_SKIP_REPEATED = 1;

/**
 * Include the log severity in messages originating from codecs.
 *
 * Results in messages such as:
 * [rawvideo @ 0xDEADBEEF] [error] encode did not produce valid pts
 */
export const AV_LOG_PRINT_LEVEL = 2;

export function av_log_set_flags(arg: number) { throw new NotImplemented(); }
export function av_log_get_flags(): number { throw new NotImplemented(); }