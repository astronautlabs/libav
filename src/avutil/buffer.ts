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

import { AVBuffer as AVBufferImpl } from "../../binding";

/**
 * AVBuffer is an API for reference-counted data buffers.
 *
 * There are two core objects in this API -- AVBuffer and AVBufferRef. AVBuffer
 * represents the data buffer itself; it is opaque and not meant to be accessed
 * by the caller directly, but only through AVBufferRef. However, the caller may
 * e.g. compare two AVBuffer pointers to check whether two different references
 * are describing the same data buffer. AVBufferRef represents a single
 * reference to an AVBuffer and it is the object that may be manipulated by the
 * caller directly.
 *
 * There are two functions provided for creating a new AVBuffer with a single
 * reference -- av_buffer_alloc() to just allocate a new buffer, and
 * av_buffer_create() to wrap an existing array in an AVBuffer. From an existing
 * reference, additional references may be created with av_buffer_ref().
 * Use av_buffer_unref() to free a reference (this will automatically free the
 * data once all the references are freed).
 *
 * The convention throughout this API and the rest of FFmpeg is such that the
 * buffer is considered writable if there exists only one reference to it (and
 * it has not been marked as read-only). The av_buffer_is_writable() function is
 * provided to check whether this is true and av_buffer_make_writable() will
 * automatically create a new writable buffer when necessary.
 * Of course nothing prevents the calling code from violating this convention,
 * however that is safe only when all the existing references are under its
 * control.
 *
 * @note Referencing and unreferencing the buffers is thread-safe and thus
 * may be done from multiple threads simultaneously without any need for
 * additional locking.
 *
 * @note Two different references to the same buffer can point to different
 * parts of the buffer (i.e. their AVBufferRef.data will not be equal).
 */
export declare class AVBuffer {

    /**
     * Construct a new AVBuffer from the given buffer.
     * @param buffer 
     */
    constructor(buffer: Uint8Array | Uint16Array | Uint32Array | BigUint64Array | ArrayBuffer | Buffer);

    /**
     * Construct a new AVBuffer with the given size.
     * @param size The size of the new buffer
     * @param zero If true, the memory will be zeroed
     */
    constructor(size: number, zero?: boolean);

    /**
     * The data buffer. It is considered writable if and only if
     * this is the only reference to the buffer, in which case
     * av_buffer_is_writable() returns 1.
     */
    readonly data: ArrayBuffer;

    /**
     * Size of data in bytes.
     */
    readonly size: number;

    /**
     * How many references exist for this AVBuffer.
     * Note that the Javascript object counts as one. 
     * Native code can also reference the underlying AVBuffer
     * to increase this count. Returns zero if the buffer has
     * been freed.
     */
    readonly refCount: number;

    /**
     * True if this buffer is writable, which is only the case
     * when refCount is no more than 1.
     */
    readonly writable: boolean;
    
    /**
     * Immediately de-reference this buffer. Note that the buffer
     * will not be immediately freed if the refrence count is greater
     * than 1 (meaning native code still has a reference to this buffer)
     */
    free();

    /**
     * Make a writable buffer from this buffer, avoiding a copy. If the buffer
     * is referenced only once (ie writable is true), then the current instance 
     * will be returned without further action. If the buffer is referenced multiple 
     * times, then a copy of this buffer will be made and returned.
     */
    makeWritable(): AVBuffer;

    
    /**
     * Reallocate a given buffer.
     *
     * @param buf  a buffer reference to reallocate. On success, buf will be
     *             unreferenced and a new reference with the required size will be
     *             written in its place. On failure buf will be left untouched. *buf
     *             may be NULL, then a new buffer is allocated.
     * @param size required new buffer size.
     * @return 0 on success, a negative AVERROR on failure.
     *
     * @note the buffer is actually reallocated with av_realloc() only if it was
     * initially allocated through av_buffer_realloc(NULL) and there is only one
     * reference to it (i.e. the one passed to this function). In all other cases
     * a new buffer is allocated and the data is copied.
     */
    realloc(size: number);

    /**
     * Ensure this buffer refers to the same data as the other buffer. When the 
     * buffers are already equivalent, do nothing.
     *
     * @param other The buffer to share data with
     */
    replace(other: AVBuffer);
}

/**
 * Always treat the buffer as read-only, even when it has only one
 * reference.
 */
export const AV_BUFFER_FLAG_READONLY = (1 << 0);

/**
 * AVBufferPool is an API for a lock-free thread-safe pool of AVBuffers.
 *
 * Frequently allocating and freeing large buffers may be slow. AVBufferPool is
 * meant to solve this in cases when the caller needs a set of buffers of the
 * same size (the most obvious use case being buffers for raw video or audio
 * frames). Allocating and releasing buffers with this API is thread-safe as long 
 * as either the default alloc callback is used, or the user-supplied one is
 * thread-safe.
 */
export declare class AVBufferPool {
    constructor(bufferSize: number, allocator?: (size: number) => AVBuffer);

    /**
     * Mark the pool as being available for freeing. It will actually be freed only
     * once all the allocated buffers associated with the pool are released. Thus it
     * is safe to call this function while some of the allocated buffers are still
     * in use.
     */
    free();

    /**
     * Allocate a new AVBuffer, reusing an old buffer from the pool when available.
     */
    get(): AVBuffer;
}
