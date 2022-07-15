/*
 * copyright (c) 2001 Fabrice Bellard
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

import { AVClass, AVDictionary } from "../avutil";
import { NotImplemented, OpaquePtr, Out, Ref } from "../helpers";
import { URLContext } from "./url";

 
/**
 * @file
 * @ingroup lavf_io
 * Buffered I/O operations
 */

/**
 * Seeking works like for a local file.
 */
const AVIO_SEEKABLE_NORMAL = (1 << 0);
 
/**
 * Seeking by timestamp with avio_seek_time() is possible.
 */
const AVIO_SEEKABLE_TIME =   (1 << 1);

/**
 * Callback for checking whether to abort blocking functions.
 * AVERROR_EXIT is returned in this case by the interrupted
 * function. During blocking operations, callback is called with
 * opaque as parameter. If the callback returns 1, the
 * blocking operation will be aborted.
 *
 * No members can be added to this struct without a major bump, if
 * new elements have been added after this struct in AVFormatContext
 * or AVIOContext.
 */
export interface AVIOInterruptCB {
    callback: (opaque: OpaquePtr) => number;
    opaque: OpaquePtr;
}
 
/**
 * Directory entry types.
 */
export enum AVIODirEntryType {
    AVIO_ENTRY_UNKNOWN,
    AVIO_ENTRY_BLOCK_DEVICE,
    AVIO_ENTRY_CHARACTER_DEVICE,
    AVIO_ENTRY_DIRECTORY,
    AVIO_ENTRY_NAMED_PIPE,
    AVIO_ENTRY_SYMBOLIC_LINK,
    AVIO_ENTRY_SOCKET,
    AVIO_ENTRY_FILE,
    AVIO_ENTRY_SERVER,
    AVIO_ENTRY_SHARE,
    AVIO_ENTRY_WORKGROUP,
};
 
/**
 * Describes single entry of the directory.
 *
 * Only name and type fields are guaranteed be set.
 * Rest of fields are protocol or/and platform dependent and might be unknown.
 */
export interface AVIODirEntry {
    name: string;                           /**< Filename */
    type: number;                             /**< Type of the entry */
    utf8: number;                             /**< Set to 1 when name is encoded with UTF-8, 0 otherwise.
                                               Name can be encoded with UTF-8 even though 0 is set. */
    size: number;                         /**< File size in bytes, -1 if unknown. */
    modification_timestamp: number;       /**< Time of last modification in microseconds since unix
                                               epoch, -1 if unknown. */
    access_timestamp: number;             /**< Time of last access in microseconds since unix epoch,
                                               -1 if unknown. */
    status_change_timestamp: number;      /**< Time of last status change in microseconds since unix
                                               epoch, -1 if unknown. */
    user_id: number;                      /**< User ID of owner, -1 if unknown. */
    group_id: number;                     /**< Group ID of owner, -1 if unknown. */
    filemode: number;                     /**< Unix file mode, -1 if unknown. */
}
 
export interface AVIODirContext {
    url_context: URLContext;
}
 
/**
 * Different data types that can be returned via the AVIO
 * write_data_type callback.
 */
export enum AVIODataMarkerType {
    /**
     * Header data; this needs to be present for the stream to be decodeable.
     */
    AVIO_DATA_MARKER_HEADER,
    /**
     * A point in the output bytestream where a decoder can start decoding
     * (i.e. a keyframe). A demuxer/decoder given the data flagged with
     * AVIO_DATA_MARKER_HEADER, followed by any AVIO_DATA_MARKER_SYNC_POINT,
     * should give decodeable results.
     */
    AVIO_DATA_MARKER_SYNC_POINT,
    /**
     * A point in the output bytestream where a demuxer can start parsing
     * (for non self synchronizing bytestream formats). That is, any
     * non-keyframe packet start point.
     */
    AVIO_DATA_MARKER_BOUNDARY_POINT,
    /**
     * This is any, unlabelled data. It can either be a muxer not marking
     * any positions at all, it can be an actual boundary/sync point
     * that the muxer chooses not to mark, or a later part of a packet/fragment
     * that is cut into multiple write callbacks due to limited IO buffer size.
     */
    AVIO_DATA_MARKER_UNKNOWN,
    /**
     * Trailer data, which doesn't contain actual content, but only for
     * finalizing the output file.
     */
    AVIO_DATA_MARKER_TRAILER,
    /**
     * A point in the output bytestream where the underlying AVIOContext might
     * flush the buffer depending on latency or buffering requirements. Typically
     * means the end of a packet.
     */
    AVIO_DATA_MARKER_FLUSH_POINT,
};
 
/**
 * Bytestream IO Context.
 * New public fields can be added with minor version bumps.
 * Removal, reordering and changes to existing public fields require
 * a major version bump.
 * sizeof(AVIOContext) must not be used outside libav*.
 *
 * @note None of the function pointers in AVIOContext should be called
 *       directly, they should only be set by the client application
 *       when implementing custom I/O. Normally these are set to the
 *       function pointers specified in avio_alloc_context()
 */
export interface AVIOContext {
    /**
     * A class for private options.
     *
     * If this AVIOContext is created by avio_open2(), av_class is set and
     * passes the options down to protocols.
     *
     * If this AVIOContext is manually allocated, then av_class may be set by
     * the caller.
     *
     * warning -- this field can be NULL, be sure to not pass this AVIOContext
     * to any av_opt_* functions in that case.
     */
    av_class: AVClass;
 
    /*
     * The following shows the relationship between buffer, buf_ptr,
     * buf_ptr_max, buf_end, buf_size, and pos, when reading and when writing
     * (since AVIOContext is used for both):
     *
     **********************************************************************************
     *                                   READING
     **********************************************************************************
     *
     *                            |              buffer_size              |
     *                            |---------------------------------------|
     *                            |                                       |
     *
     *                         buffer          buf_ptr       buf_end
     *                            +---------------+-----------------------+
     *                            |/ / / / / / / /|/ / / / / / /|         |
     *  read buffer:              |/ / consumed / | to be read /|         |
     *                            |/ / / / / / / /|/ / / / / / /|         |
     *                            +---------------+-----------------------+
     *
     *                                                         pos
     *              +-------------------------------------------+-----------------+
     *  input file: |                                           |                 |
     *              +-------------------------------------------+-----------------+
     *
     *
     **********************************************************************************
     *                                   WRITING
     **********************************************************************************
     *
     *                             |          buffer_size                 |
     *                             |--------------------------------------|
     *                             |                                      |
     *
     *                                                buf_ptr_max
     *                          buffer                 (buf_ptr)       buf_end
     *                             +-----------------------+--------------+
     *                             |/ / / / / / / / / / / /|              |
     *  write buffer:              | / / to be flushed / / |              |
     *                             |/ / / / / / / / / / / /|              |
     *                             +-----------------------+--------------+
     *                               buf_ptr can be in this
     *                               due to a backward seek
     *
     *                            pos
     *               +-------------+----------------------------------------------+
     *  output file: |             |                                              |
     *               +-------------+----------------------------------------------+
     *
     */
    buffer: Buffer;  /**< Start of the buffer. */
    buffer_size: number;        /**< Maximum buffer size */
    buf_ptr: Buffer; /**< Current position in the buffer */
    buf_end: OpaquePtr; /**< End of the data, may be less than
                                 buffer+buffer_size if the read function returned
                                 less data than requested, e.g. for streams where
                                 no more data has been received yet. */
    opaque: OpaquePtr;           /**< A private pointer, passed to the read/write/seek/...
                                 functions. */

    read_packet: (opaque: OpaquePtr, buf: Buffer, buf_size: number) => number;
    write_packet: (opaque: OpaquePtr, buf: Buffer, buf_size: number) => number;
    seek: (opaque: OpaquePtr, offset: number, whence: number) => number;

    /**< position in the file of the current buffer */
    pos: number;

    /**< true if was unable to read due to error or eof */
    eof_reached: number;

    /**< contains the error code or 0 if no error happened */
    error: number;

    /**< true if open for writing */
    write_flag: number;

    max_packet_size: number;

    /**< Try to buffer at least this amount of data
                                 before flushing it. */
    min_packet_size: number;

    checksum: number;

    /**
     * @nodeTODO unsigned int*
     */
    checksum_ptr: OpaquePtr;

    update_checksum: (checksum: number, buf: Buffer, size: number) => number;

    /**
     * Pause or resume playback for network streaming protocols - e.g. MMS.
     */
    read_pause: (opaque: OpaquePtr, pause: number) => number;

    /**
     * Seek to a given timestamp in stream with the specified stream_index.
     * Needed for some network streaming protocols which don't support seeking
     * to byte position.
     */
    read_seek: (opaque: OpaquePtr, stream_index: number, timestamp: number, flags: number) => number;

    /**
     * A combination of AVIO_SEEKABLE_ flags or 0 when the stream is not seekable.
     */
    seekable: number;
 
    /**
     * avio_read and avio_write should if possible be satisfied directly
     * instead of going through a buffer, and avio_seek will always
     * call the underlying seek function directly.
     */
    direct: number;
 
    /**
     * ',' separated list of allowed protocols.
     */
    protocol_whitelist: string;
 
    /**
     * ',' separated list of disallowed protocols.
     */
    protocol_blacklist: string;
 
    /**
     * A callback that is used instead of write_packet.
     */
    write_data_type: (opaque: OpaquePtr, buf: Buffer, buf_size: number, type: AVIODataMarkerType, time: number) => number;

    /**
     * If set, don't call write_data_type separately for AVIO_DATA_MARKER_BOUNDARY_POINT,
     * but ignore them and treat them as AVIO_DATA_MARKER_UNKNOWN (to avoid needlessly
     * small chunks of data returned from the callback).
     */
    ignore_boundary_point: number;
 
    /**
     * @deprecated field utilized privately by libavformat. For a public
     *             statistic of how many bytes were written out, see
     *             AVIOContext::bytes_written.
     * @if FF_API_AVIOCONTEXT_WRITTEN
     * @attribute_deprecated
     */
    written: number;
 
    /**
     * Maximum reached position before a backward seek in the write buffer,
     * used keeping track of already written data for a later flush.
     * @nodeTODO unsigned char *
     */
    buf_ptr_max: OpaquePtr;
 
    /**
     * Read-only statistic of bytes read for this AVIOContext.
     */
    bytes_read: number;
 
    /**
     * Read-only statistic of bytes written for this AVIOContext.
     */
    bytes_written: number;
}
 
/**
 * Return the name of the protocol that will handle the passed URL.
 *
 * NULL is returned if no protocol could be found for the given URL.
 *
 * @return Name of the protocol or NULL.
 */
export function avio_find_protocol_name(url: string): string { throw new NotImplemented(); }
 
/**
 * Return AVIO_FLAG_* access flags corresponding to the access permissions
 * of the resource in url, or a negative value corresponding to an
 * AVERROR code in case of failure. The returned access flags are
 * masked by the value in flags.
 *
 * @note This function is intrinsically unsafe, in the sense that the
 * checked resource may change its existence or permission status from
 * one call to another. Thus you should not trust the returned value,
 * unless you are sure that no other processes are accessing the
 * checked resource.
 */
export function avio_check(url: string, flags: number): number { throw new NotImplemented(); }
 
/**
 * Open directory for reading.
 *
 * @param s       directory read context. Pointer to a NULL pointer must be passed.
 * @param url     directory to be listed.
 * @param options A dictionary filled with protocol-private options. On return
 *                this parameter will be destroyed and replaced with a dictionary
 *                containing options that were not found. May be NULL.
 * @return >=0 on success or negative on error.
 */
export function avio_open_dir(s: Out<AVIODirContext>, url: string, options: Ref<AVDictionary>): number { throw new NotImplemented(); }
 
/**
 * Get next directory entry.
 *
 * Returned entry must be freed with avio_free_directory_entry(). In particular
 * it may outlive AVIODirContext.
 *
 * @param s         directory read context.
 * @param[out] next next entry or NULL when no more entries.
 * @return >=0 on success or negative on error. End of list is not considered an
 *             error.
 */
export function avio_read_dir(s: AVIODirContext, next: Out<AVIODirEntry>): number { throw new NotImplemented(); }
 
/**
 * Close directory.
 *
 * @note Entries created using avio_read_dir() are not deleted and must be
 * freeded with avio_free_directory_entry().
 *
 * @param s         directory read context.
 * @return >=0 on success or negative on error.
 */
export function avio_close_dir(s: Ref<AVIODirContext>): number { throw new NotImplemented(); }
 
/**
 * Free entry allocated by avio_read_dir().
 *
 * @param entry entry to be freed.
 */
export function avio_free_directory_entry(entry: Ref<AVIODirEntry>) { throw new NotImplemented(); }
 
/**
 * Allocate and initialize an AVIOContext for buffered I/O. It must be later
 * freed with avio_context_free().
 *
 * @param buffer Memory block for input/output operations via AVIOContext.
 *        The buffer must be allocated with av_malloc() and friends.
 *        It may be freed and replaced with a new buffer by libavformat.
 *        AVIOContext.buffer holds the buffer currently in use,
 *        which must be later freed with av_free().
 * @param buffer_size The buffer size is very important for performance.
 *        For protocols with fixed blocksize it should be set to this blocksize.
 *        For others a typical size is a cache page, e.g. 4kb.
 * @param write_flag Set to 1 if the buffer should be writable, 0 otherwise.
 * @param opaque An opaque pointer to user-specific data.
 * @param read_packet  A function for refilling the buffer, may be NULL.
 *                     For stream protocols, must never return 0 but rather
 *                     a proper AVERROR code.
 * @param write_packet A function for writing the buffer contents, may be NULL.
 *        The function may not change the input buffers content.
 * @param seek A function for seeking to specified byte position, may be NULL.
 *
 * @return Allocated AVIOContext or NULL on failure.
 */
export function avio_alloc_context(
                  buffer: Buffer,
                  buffer_size: number,
                  write_flag: number,
                  opaque: OpaquePtr,
                  read_packet: (opaque: OpaquePtr, buf: Buffer, buf_size: number) => number,
                  write_packet: (opaque: OpaquePtr, buf: Buffer, buf_size: number) => number,
                  seek: (opaque: OpaquePtr, offset: number, whence: number) => number
): AVIOContext { throw new NotImplemented(); }
 
/**
 * Free the supplied IO context and everything associated with it.
 *
 * @param s Double pointer to the IO context. This function will write NULL
 * into s.
 */
export function avio_context_free(s: Ref<AVIOContext>) {}
export function avio_w8(s: AVIOContext, b: number) {}
export function avio_write(s: AVIOContext, buf: Buffer, size: number) {}
export function avio_wl64(s: AVIOContext, val: number) {}
export function avio_wb64(s: AVIOContext, val: number) {}
export function avio_wl32(s: AVIOContext, val: number) {}
export function avio_wb32(s: AVIOContext, val: number) {}
export function avio_wl24(s: AVIOContext, val: number) {}
export function avio_wb24(s: AVIOContext, val: number) {}
export function avio_wl16(s: AVIOContext, val: number) {}
export function avio_wb16(s: AVIOContext, val: number) {}
 
/**
 * Write a NULL-terminated string.
 * @return number of bytes written.
 */
export function avio_put_str(s: AVIOContext, str: string) { throw new NotImplemented(); }
 
/**
 * Convert an UTF-8 string to UTF-16LE and write it.
 * @param s the AVIOContext
 * @param str NULL-terminated UTF-8 string
 *
 * @return number of bytes written.
 */
export function avio_put_str16le(s: AVIOContext, str: string): number { throw new NotImplemented(); }
 
/**
 * Convert an UTF-8 string to UTF-16BE and write it.
 * @param s the AVIOContext
 * @param str NULL-terminated UTF-8 string
 *
 * @return number of bytes written.
 */
export function avio_put_str16be(s: AVIOContext, str: string): number { throw new NotImplemented(); }
 
/**
 * Mark the written bytestream as a specific type.
 *
 * Zero-length ranges are omitted from the output.
 *
 * @param time the stream time the current bytestream pos corresponds to
 *             (in AV_TIME_BASE units), or AV_NOPTS_VALUE if unknown or not
 *             applicable
 * @param type the kind of data written starting at the current pos
 */
export function avio_write_marker(s: AVIOContext, time: number, type: AVIODataMarkerType) { throw new NotImplemented(); }
 
/**
 * ORing this as the "whence" parameter to a seek function causes it to
 * return the filesize without seeking anywhere. Supporting this is optional.
 * If it is not supported then the seek function will return <0.
 */
export const AVSEEK_SIZE = 0x10000;
 
/**
 * Passing this flag as the "whence" parameter to a seek function causes it to
 * seek by any means (like reopening and linear reading) or other normally unreasonable
 * means that can be extremely slow.
 * This may be ignored by the seek code.
 */
export const AVSEEK_FORCE = 0x20000;
 
/**
 * fseek() equivalent for AVIOContext.
 * @return new position or AVERROR.
 */
export function avio_seek(s: AVIOContext, offset: number, whence: number): number { throw new NotImplemented(); }
 
/**
 * Skip given number of bytes forward
 * @return new position or AVERROR.
 */
export function avio_skip(s: AVIOContext, offset: number): number { throw new NotImplemented(); }
 
/**
 * ftell() equivalent for AVIOContext.
 * @return position or AVERROR.
 */
export function avio_tell(s: AVIOContext): number
{
    return avio_seek(s, 0, 1/*SEEK_CUR*/);
}
 
/**
 * Get the filesize.
 * @return filesize or AVERROR
 */
export function avio_size(s: AVIOContext): number { throw new NotImplemented(); }
 
/**
 * Similar to feof() but also returns nonzero on read errors.
 * @return non zero if and only if at end of file or a read error happened when reading.
 */
export function avio_feof(s: AVIOContext): number { throw new NotImplemented(); }
 
/**
 * Writes a formatted string to the context taking a va_list.
 * @return number of bytes written, < 0 on error.
 */
export function avio_vprintf(s: AVIOContext, fmt: string, ...ap): number { throw new NotImplemented(); }
 
/**
 * Writes a formatted string to the context.
 * @return number of bytes written, < 0 on error.
 */
export function avio_printf(s: AVIOContext, fmt: string, ...args): number { throw new NotImplemented(); }
 
/**
 * Write a NULL terminated array of strings to the context.
 * Usually you don't need to use this function directly but its macro wrapper,
 * avio_print.
 */
export function avio_print_string_array(s: AVIOContext, strings: string[]) { throw new NotImplemented(); }
 
/**
 * Write strings (const char *) to the context.
 * This is a convenience macro around avio_print_string_array and it
 * automatically creates the string array from the variable argument list.
 * For simple string concatenations this function is more performant than using
 * avio_printf since it does not need a temporary buffer.
 */
export function avio_print(s, ...args) {
    avio_print_string_array(s, [...args, null]);
}
 
/**
 * Force flushing of buffered data.
 *
 * For write streams, force the buffered data to be immediately written to the output,
 * without to wait to fill the internal buffer.
 *
 * For read streams, discard all currently buffered data, and advance the
 * reported file position to that of the underlying stream. This does not
 * read new data, and does not perform any seeks.
 */
export function avio_flush(s: AVIOContext) { throw new NotImplemented(); }
 
/**
 * Read size bytes from AVIOContext into buf.
 * @return number of bytes read or AVERROR
 */
export function avio_read(s: AVIOContext, buf: Buffer, size: number): number { throw new NotImplemented(); }
 
/**
 * Read size bytes from AVIOContext into buf. Unlike avio_read(), this is allowed
 * to read fewer bytes than requested. The missing bytes can be read in the next
 * call. This always tries to read at least 1 byte.
 * Useful to reduce latency in certain cases.
 * @return number of bytes read or AVERROR
 */
export function avio_read_partial(s: AVIOContext, buf: Buffer, size: number): number { throw new NotImplemented(); }
 
/**
 * @name Functions for reading from AVIOContext
 * @{
 *
 * @note return 0 if EOF, so you cannot use it if EOF handling is
 *       necessary
 */
export function avio_r8  (s: AVIOContext): number { throw new NotImplemented(); }
export function avio_rl16(s: AVIOContext): number { throw new NotImplemented(); }
export function avio_rl24(s: AVIOContext): number { throw new NotImplemented(); }
export function avio_rl32(s: AVIOContext): number { throw new NotImplemented(); }
export function avio_rl64(s: AVIOContext): number { throw new NotImplemented(); }
export function avio_rb16(s: AVIOContext): number { throw new NotImplemented(); }
export function avio_rb24(s: AVIOContext): number { throw new NotImplemented(); }
export function avio_rb32(s: AVIOContext): number { throw new NotImplemented(); }
export function avio_rb64(s: AVIOContext): number { throw new NotImplemented(); }

/**
 * @}
 */
 
/**
 * Read a string from pb into buf. The reading will terminate when either
 * a NULL character was encountered, maxlen bytes have been read, or nothing
 * more can be read from pb. The result is guaranteed to be NULL-terminated, it
 * will be truncated if buf is too small.
 * Note that the string is not interpreted or validated in any way, it
 * might get truncated in the middle of a sequence for multi-byte encodings.
 *
 * @return number of bytes read (is always <= maxlen).
 * If reading ends on EOF or error, the return value will be one more than
 * bytes actually read.
 */
export function avio_get_str(pb: AVIOContext, maxlen: number, buf: Buffer, buflen: number): number { throw new NotImplemented(); }
 
/**
 * Read a UTF-16 string from pb and convert it to UTF-8.
 * The reading will terminate when either a null or invalid character was
 * encountered or maxlen bytes have been read.
 * @return number of bytes read (is always <= maxlen)
 */
export function avio_get_str16le(pb: AVIOContext, maxlen: number, buf: Buffer, buflen: number): number { throw new NotImplemented(); }
export function avio_get_str16be(pb: AVIOContext, maxlen: number, buf: Buffer, buflen: number): number { throw new NotImplemented(); }
 
 
/**
 * @name URL open modes
 * The flags argument to avio_open must be one of the following
 * constants, optionally ORed with other flags.
 * @{
 */

/**< read-only */
const AVIO_FLAG_READ =  1;

/**< write-only */
const AVIO_FLAG_WRITE = 2;

/**< read-write pseudo flag */
const AVIO_FLAG_READ_WRITE = (AVIO_FLAG_READ|AVIO_FLAG_WRITE);

/**
 * @}
 */
 
/**
 * Use non-blocking mode.
 * If this flag is set, operations on the context will return
 * AVERROR(EAGAIN) if they can not be performed immediately.
 * If this flag is not set, operations on the context will never return
 * AVERROR(EAGAIN).
 * Note that this flag does not affect the opening/connecting of the
 * context. Connecting a protocol will always block if necessary (e.g. on
 * network protocols) but never hang (e.g. on busy devices).
 * Warning: non-blocking protocols is work-in-progress; this flag may be
 * silently ignored.
 */
export const AVIO_FLAG_NONBLOCK = 8
 
/**
 * Use direct mode.
 * avio_read and avio_write should if possible be satisfied directly
 * instead of going through a buffer, and avio_seek will always
 * call the underlying seek function directly.
 */
export const AVIO_FLAG_DIRECT = 0x8000
 
/**
 * Create and initialize a AVIOContext for accessing the
 * resource indicated by url.
 * @note When the resource indicated by url has been opened in
 * read+write mode, the AVIOContext can be used only for writing.
 *
 * @param s Used to return the pointer to the created AVIOContext.
 * In case of failure the pointed to value is set to NULL.
 * @param url resource to access
 * @param flags flags which control how the resource indicated by url
 * is to be opened
 * @return >= 0 in case of success, a negative value corresponding to an
 * AVERROR code in case of failure
 */
export function avio_open(s: Out<AVIOContext>, url: string, flags: number): number { throw new NotImplemented(); }
 
/**
 * Create and initialize a AVIOContext for accessing the
 * resource indicated by url.
 * @note When the resource indicated by url has been opened in
 * read+write mode, the AVIOContext can be used only for writing.
 *
 * @param s Used to return the pointer to the created AVIOContext.
 * In case of failure the pointed to value is set to NULL.
 * @param url resource to access
 * @param flags flags which control how the resource indicated by url
 * is to be opened
 * @param int_cb an interrupt callback to be used at the protocols level
 * @param options  A dictionary filled with protocol-private options. On return
 * this parameter will be destroyed and replaced with a dict containing options
 * that were not found. May be NULL.
 * @return >= 0 in case of success, a negative value corresponding to an
 * AVERROR code in case of failure
 */
export function avio_open2(s: Out<AVIOContext>, url: string, flags: number,
               int_cb: AVIOInterruptCB, options: Ref<AVDictionary>): number { throw new NotImplemented(); }
 
/**
 * Close the resource accessed by the AVIOContext s and free it.
 * This function can only be used if s was opened by avio_open().
 *
 * The internal buffer is automatically flushed before closing the
 * resource.
 *
 * @return 0 on success, an AVERROR < 0 on error.
 * @see avio_closep
 */
export function avio_close(s: AVIOContext): number { throw new NotImplemented(); }
 
/**
 * Close the resource accessed by the AVIOContext *s, free it
 * and set the pointer pointing to it to NULL.
 * This function can only be used if s was opened by avio_open().
 *
 * The internal buffer is automatically flushed before closing the
 * resource.
 *
 * @return 0 on success, an AVERROR < 0 on error.
 * @see avio_close
 */
export function avio_closep(s: Out<AVIOContext>): number { throw new NotImplemented(); }
 
 
/**
 * Open a write only memory stream.
 *
 * @param s new IO context
 * @return zero if no error.
 */
export function avio_open_dyn_buf(s: Out<AVIOContext>): number { throw new NotImplemented(); }
 
/**
 * Return the written size and a pointer to the buffer.
 * The AVIOContext stream is left intact.
 * The buffer must NOT be freed.
 * No padding is added to the buffer.
 *
 * @param s IO context
 * @param pbuffer pointer to a byte buffer
 * @return the length of the byte buffer
 */
export function avio_get_dyn_buf(s: AVIOContext, pbuffer: Out<Buffer>): number { throw new NotImplemented(); }
 
/**
 * Return the written size and a pointer to the buffer. The buffer
 * must be freed with av_free().
 * Padding of AV_INPUT_BUFFER_PADDING_SIZE is added to the buffer.
 *
 * @param s IO context
 * @param pbuffer pointer to a byte buffer
 * @return the length of the byte buffer
 */
export function avio_close_dyn_buf(s: AVIOContext, pbuffer: Out<Buffer>): number { throw new NotImplemented(); }
 
/**
 * Iterate through names of available protocols.
 *
 * @param opaque A private pointer representing current protocol.
 *        It must be a pointer to NULL on first iteration and will
 *        be updated by successive calls to avio_enum_protocols.
 * @param output If set to 1, iterate over output protocols,
 *               otherwise over input protocols.
 *
 * @return A static string containing the name of current protocol or NULL
 */
export function avio_enum_protocols(opaque: Ref<OpaquePtr>, output: number): string { throw new NotImplemented(); }
 
/**
 * Get AVClass by names of available protocols.
 *
 * @return A AVClass of input protocol name or NULL
 */
export function avio_protocol_get_class(name: string): AVClass { throw new NotImplemented(); }
 
/**
 * Pause and resume playing - only meaningful if using a network streaming
 * protocol (e.g. MMS).
 *
 * @param h     IO context from which to call the read_pause function pointer
 * @param pause 1 for pause, 0 for resume
 */
export function avio_pause(h: AVIOContext, pause: number): number { throw new NotImplemented(); }
 
/**
 * Seek to a given timestamp relative to some component stream.
 * Only meaningful if using a network streaming protocol (e.g. MMS.).
 *
 * @param h IO context from which to call the seek function pointers
 * @param stream_index The stream index that the timestamp is relative to.
 *        If stream_index is (-1) the timestamp should be in AV_TIME_BASE
 *        units from the beginning of the presentation.
 *        If a stream_index >= 0 is used and the protocol does not support
 *        seeking based on component streams, the call will fail.
 * @param timestamp timestamp in AVStream.time_base units
 *        or if there is no stream specified then in AV_TIME_BASE units.
 * @param flags Optional combination of AVSEEK_FLAG_BACKWARD, AVSEEK_FLAG_BYTE
 *        and AVSEEK_FLAG_ANY. The protocol may silently ignore
 *        AVSEEK_FLAG_BACKWARD and AVSEEK_FLAG_ANY, but AVSEEK_FLAG_BYTE will
 *        fail if used and not supported.
 * @return >= 0 on success
 * @see AVInputFormat::read_seek
 */
export function avio_seek_time(h: AVIOContext, stream_index: number,
                       timestamp: number, flags: number): number { throw new NotImplemented(); }
 
/* Avoid a warning. The header can not be included because it breaks c++. */
export type AVBPrint = OpaquePtr; // @nodeTODO 
 
/**
 * Read contents of h into print buffer, up to max_size bytes, or up to EOF.
 *
 * @return 0 for success (max_size bytes read or EOF reached), negative error
 * code otherwise
 */
export function avio_read_to_bprint(h: AVIOContext, pb: AVBPrint, max_size: number): number { throw new NotImplemented(); }
 
/**
 * Accept and allocate a client context on a server context.
 * @param  s the server context
 * @param  c the client context, must be unallocated
 * @return   >= 0 on success or a negative value corresponding
 *           to an AVERROR on failure
 */
export function avio_accept(s: AVIOContext, c: Out<AVIOContext>): number { throw new NotImplemented(); }
 
/**
 * Perform one step of the protocol handshake to accept a new client.
 * This function must be called on a client returned by avio_accept() before
 * using it as a read/write context.
 * It is separate from avio_accept() because it may block.
 * A step of the handshake is defined by places where the application may
 * decide to change the proceedings.
 * For example, on a protocol with a request header and a reply header, each
 * one can constitute a step because the application may use the parameters
 * from the request to change parameters in the reply; or each individual
 * chunk of the request can constitute a step.
 * If the handshake is already finished, avio_handshake() does nothing and
 * returns 0 immediately.
 *
 * @param  c the client context to perform the handshake on
 * @return   0   on a complete and successful handshake
 *           > 0 if the handshake progressed, but is not complete
 *           < 0 for an AVERROR code
 */
export function avio_handshake(c: AVIOContext): number { throw new NotImplemented(); }