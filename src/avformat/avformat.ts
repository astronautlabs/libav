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

import { AVCodec, AVCodecID, AVCodecParameters, AVCodecParserContext, AVDiscard, AVPacket, AVPacketSideData, AVPacketSideDataType } from "../avcodec";
import { AVDeviceInfoList } from "../avdevice";
import { AVClass, AVDictionary, AVFrame, AVMediaType, AVRational } from "../avutil";
import { FILE, OpaquePtr, Out, Ref } from "../helpers";
import { AVIOContext, AVIOInterruptCB } from "./avio";

 
/**
 * @file
 * @ingroup libavf
 * Main libavformat public API header
 */
 
/**
 * @defgroup libavf libavformat
 * I/O and Muxing/Demuxing Library
 *
 * Libavformat (lavf) is a library for dealing with various media container
 * formats. Its main two purposes are demuxing - i.e. splitting a media file
 * into component streams, and the reverse process of muxing - writing supplied
 * data in a specified container format. It also has an @ref lavf_io
 * "I/O module" which supports a number of protocols for accessing the data (e.g.
 * file, tcp, http and others).
 * Unless you are absolutely sure you won't use libavformat's network
 * capabilities, you should also call avformat_network_init().
 *
 * A supported input format is described by an AVInputFormat struct, conversely
 * an output format is described by AVOutputFormat. You can iterate over all
 * input/output formats using the  av_demuxer_iterate / av_muxer_iterate() functions.
 * The protocols layer is not part of the public API, so you can only get the names
 * of supported protocols with the avio_enum_protocols() function.
 *
 * Main lavf structure used for both muxing and demuxing is AVFormatContext,
 * which exports all information about the file being read or written. As with
 * most Libavformat structures, its size is not part of public ABI, so it cannot be
 * allocated on stack or directly with av_malloc(). To create an
 * AVFormatContext, use avformat_alloc_context() (some functions, like
 * avformat_open_input() might do that for you).
 *
 * Most importantly an AVFormatContext contains:
 * @li the @ref AVFormatContext.iformat "input" or @ref AVFormatContext.oformat
 * "output" format. It is either autodetected or set by user for input;
 * always set by user for output.
 * @li an @ref AVFormatContext.streams "array" of AVStreams, which describe all
 * elementary streams stored in the file. AVStreams are typically referred to
 * using their index in this array.
 * @li an @ref AVFormatContext.pb "I/O context". It is either opened by lavf or
 * set by user for input, always set by user for output (unless you are dealing
 * with an AVFMT_NOFILE format).
 *
 * @section lavf_options Passing options to (de)muxers
 * It is possible to configure lavf muxers and demuxers using the @ref avoptions
 * mechanism. Generic (format-independent) libavformat options are provided by
 * AVFormatContext, they can be examined from a user program by calling
 * av_opt_next() / av_opt_find() on an allocated AVFormatContext (or its AVClass
 * from avformat_get_class()). Private (format-specific) options are provided by
 * AVFormatContext.priv_data if and only if AVInputFormat.priv_class /
 * AVOutputFormat.priv_class of the corresponding format struct is non-NULL.
 * Further options may be provided by the @ref AVFormatContext.pb "I/O context",
 * if its AVClass is non-NULL, and the protocols layer. See the discussion on
 * nesting in @ref avoptions documentation to learn how to access those.
 *
 * @section urls
 * URL strings in libavformat are made of a scheme/protocol, a ':', and a
 * scheme specific string. URLs without a scheme and ':' used for local files
 * are supported but deprecated. "file:" should be used for local files.
 *
 * It is important that the scheme string is not taken from untrusted
 * sources without checks.
 *
 * Note that some schemes/protocols are quite powerful, allowing access to
 * both local and remote files, parts of them, concatenations of them, local
 * audio and video devices and so on.
 *
 * @{
 *
 * @defgroup lavf_decoding Demuxing
 * @{
 * Demuxers read a media file and split it into chunks of data (@em packets). A
 * @ref AVPacket "packet" contains one or more encoded frames which belongs to a
 * single elementary stream. In the lavf API this process is represented by the
 * avformat_open_input() function for opening a file, av_read_frame() for
 * reading a single packet and finally avformat_close_input(), which does the
 * cleanup.
 *
 * @section lavf_decoding_open Opening a media file
 * The minimum information required to open a file is its URL, which
 * is passed to avformat_open_input(), as in the following code:
 * @code
 * const char    *url = "file:in.mp3";
 * AVFormatContext *s = NULL;
 * int ret = avformat_open_input(&s, url, NULL, NULL);
 * if (ret < 0)
 *     abort();
 * @endcode
 * The above code attempts to allocate an AVFormatContext, open the
 * specified file (autodetecting the format) and read the header, exporting the
 * information stored there into s. Some formats do not have a header or do not
 * store enough information there, so it is recommended that you call the
 * avformat_find_stream_info() function which tries to read and decode a few
 * frames to find missing information.
 *
 * In some cases you might want to preallocate an AVFormatContext yourself with
 * avformat_alloc_context() and do some tweaking on it before passing it to
 * avformat_open_input(). One such case is when you want to use custom functions
 * for reading input data instead of lavf internal I/O layer.
 * To do that, create your own AVIOContext with avio_alloc_context(), passing
 * your reading callbacks to it. Then set the @em pb field of your
 * AVFormatContext to newly created AVIOContext.
 *
 * Since the format of the opened file is in general not known until after
 * avformat_open_input() has returned, it is not possible to set demuxer private
 * options on a preallocated context. Instead, the options should be passed to
 * avformat_open_input() wrapped in an AVDictionary:
 * @code
 * AVDictionary *options = NULL;
 * av_dict_set(&options, "video_size", "640x480", 0);
 * av_dict_set(&options, "pixel_format", "rgb24", 0);
 *
 * if (avformat_open_input(&s, url, NULL, &options) < 0)
 *     abort();
 * av_dict_free(&options);
 * @endcode
 * This code passes the private options 'video_size' and 'pixel_format' to the
 * demuxer. They would be necessary for e.g. the rawvideo demuxer, since it
 * cannot know how to interpret raw video data otherwise. If the format turns
 * out to be something different than raw video, those options will not be
 * recognized by the demuxer and therefore will not be applied. Such unrecognized
 * options are then returned in the options dictionary (recognized options are
 * consumed). The calling program can handle such unrecognized options as it
 * wishes, e.g.
 * @code
 * AVDictionaryEntry *e;
 * if (e = av_dict_get(options, "", NULL, AV_DICT_IGNORE_SUFFIX)) {
 *     fprintf(stderr, "Option %s not recognized by the demuxer.\n", e->key);
 *     abort();
 * }
 * @endcode
 *
 * After you have finished reading the file, you must close it with
 * avformat_close_input(). It will free everything associated with the file.
 *
 * @section lavf_decoding_read Reading from an opened file
 * Reading data from an opened AVFormatContext is done by repeatedly calling
 * av_read_frame() on it. Each call, if successful, will return an AVPacket
 * containing encoded data for one AVStream, identified by
 * AVPacket.stream_index. This packet may be passed straight into the libavcodec
 * decoding functions avcodec_send_packet() or avcodec_decode_subtitle2() if the
 * caller wishes to decode the data.
 *
 * AVPacket.pts, AVPacket.dts and AVPacket.duration timing information will be
 * set if known. They may also be unset (i.e. AV_NOPTS_VALUE for
 * pts/dts, 0 for duration) if the stream does not provide them. The timing
 * information will be in AVStream.time_base units, i.e. it has to be
 * multiplied by the timebase to convert them to seconds.
 *
 * A packet returned by av_read_frame() is always reference-counted,
 * i.e. AVPacket.buf is set and the user may keep it indefinitely.
 * The packet must be freed with av_packet_unref() when it is no
 * longer needed.
 *
 * @section lavf_decoding_seek Seeking
 * @}
 *
 * @defgroup lavf_encoding Muxing
 * @{
 * Muxers take encoded data in the form of @ref AVPacket "AVPackets" and write
 * it into files or other output bytestreams in the specified container format.
 *
 * The main API functions for muxing are avformat_write_header() for writing the
 * file header, av_write_frame() / av_interleaved_write_frame() for writing the
 * packets and av_write_trailer() for finalizing the file.
 *
 * At the beginning of the muxing process, the caller must first call
 * avformat_alloc_context() to create a muxing context. The caller then sets up
 * the muxer by filling the various fields in this context:
 *
 * - The @ref AVFormatContext.oformat "oformat" field must be set to select the
 *   muxer that will be used.
 * - Unless the format is of the AVFMT_NOFILE type, the @ref AVFormatContext.pb
 *   "pb" field must be set to an opened IO context, either returned from
 *   avio_open2() or a custom one.
 * - Unless the format is of the AVFMT_NOSTREAMS type, at least one stream must
 *   be created with the avformat_new_stream() function. The caller should fill
 *   the @ref AVStream.codecpar "stream codec parameters" information, such as the
 *   codec @ref AVCodecParameters.codec_type "type", @ref AVCodecParameters.codec_id
 *   "id" and other parameters (e.g. width / height, the pixel or sample format,
 *   etc.) as known. The @ref AVStream.time_base "stream timebase" should
 *   be set to the timebase that the caller desires to use for this stream (note
 *   that the timebase actually used by the muxer can be different, as will be
 *   described later).
 * - It is advised to manually initialize only the relevant fields in
 *   AVCodecParameters, rather than using @ref avcodec_parameters_copy() during
 *   remuxing: there is no guarantee that the codec context values remain valid
 *   for both input and output format contexts.
 * - The caller may fill in additional information, such as @ref
 *   AVFormatContext.metadata "global" or @ref AVStream.metadata "per-stream"
 *   metadata, @ref AVFormatContext.chapters "chapters", @ref
 *   AVFormatContext.programs "programs", etc. as described in the
 *   AVFormatContext documentation. Whether such information will actually be
 *   stored in the output depends on what the container format and the muxer
 *   support.
 *
 * When the muxing context is fully set up, the caller must call
 * avformat_write_header() to initialize the muxer internals and write the file
 * header. Whether anything actually is written to the IO context at this step
 * depends on the muxer, but this function must always be called. Any muxer
 * private options must be passed in the options parameter to this function.
 *
 * The data is then sent to the muxer by repeatedly calling av_write_frame() or
 * av_interleaved_write_frame() (consult those functions' documentation for
 * discussion on the difference between them; only one of them may be used with
 * a single muxing context, they should not be mixed). Do note that the timing
 * information on the packets sent to the muxer must be in the corresponding
 * AVStream's timebase. That timebase is set by the muxer (in the
 * avformat_write_header() step) and may be different from the timebase
 * requested by the caller.
 *
 * Once all the data has been written, the caller must call av_write_trailer()
 * to flush any buffered packets and finalize the output file, then close the IO
 * context (if any) and finally free the muxing context with
 * avformat_free_context().
 * @}
 *
 * @defgroup lavf_io I/O Read/Write
 * @{
 * @section lavf_io_dirlist Directory listing
 * The directory listing API makes it possible to list files on remote servers.
 *
 * Some of possible use cases:
 * - an "open file" dialog to choose files from a remote location,
 * - a recursive media finder providing a player with an ability to play all
 * files from a given directory.
 *
 * @subsection lavf_io_dirlist_open Opening a directory
 * At first, a directory needs to be opened by calling avio_open_dir()
 * supplied with a URL and, optionally, ::AVDictionary containing
 * protocol-specific parameters. The function returns zero or positive
 * integer and allocates AVIODirContext on success.
 *
 * @code
 * AVIODirContext *ctx = NULL;
 * if (avio_open_dir(&ctx, "smb://example.com/some_dir", NULL) < 0) {
 *     fprintf(stderr, "Cannot open directory.\n");
 *     abort();
 * }
 * @endcode
 *
 * This code tries to open a sample directory using smb protocol without
 * any additional parameters.
 *
 * @subsection lavf_io_dirlist_read Reading entries
 * Each directory's entry (i.e. file, another directory, anything else
 * within ::AVIODirEntryType) is represented by AVIODirEntry.
 * Reading consecutive entries from an opened AVIODirContext is done by
 * repeatedly calling avio_read_dir() on it. Each call returns zero or
 * positive integer if successful. Reading can be stopped right after the
 * NULL entry has been read -- it means there are no entries left to be
 * read. The following code reads all entries from a directory associated
 * with ctx and prints their names to standard output.
 * @code
 * AVIODirEntry *entry = NULL;
 * for (;;) {
 *     if (avio_read_dir(ctx, &entry) < 0) {
 *         fprintf(stderr, "Cannot list directory.\n");
 *         abort();
 *     }
 *     if (!entry)
 *         break;
 *     printf("%s\n", entry->name);
 *     avio_free_directory_entry(&entry);
 * }
 * @endcode
 * @}
 *
 * @defgroup lavf_codec Demuxers
 * @{
 * @defgroup lavf_codec_native Native Demuxers
 * @{
 * @}
 * @defgroup lavf_codec_wrappers External library wrappers
 * @{
 * @}
 * @}
 * @defgroup lavf_protos I/O Protocols
 * @{
 * @}
 * @defgroup lavf_internal Internal
 * @{
 * @}
 * @}
 */
 
/**
 * @defgroup metadata_api Public Metadata API
 * @{
 * @ingroup libavf
 * The metadata API allows libavformat to export metadata tags to a client
 * application when demuxing. Conversely it allows a client application to
 * set metadata when muxing.
 *
 * Metadata is exported or set as pairs of key/value strings in the 'metadata'
 * fields of the AVFormatContext, AVStream, AVChapter and AVProgram structs
 * using the @ref lavu_dict "AVDictionary" API. Like all strings in FFmpeg,
 * metadata is assumed to be UTF-8 encoded Unicode. Note that metadata
 * exported by demuxers isn't checked to be valid UTF-8 in most cases.
 *
 * Important concepts to keep in mind:
 * -  Keys are unique; there can never be 2 tags with the same key. This is
 *    also meant semantically, i.e., a demuxer should not knowingly produce
 *    several keys that are literally different but semantically identical.
 *    E.g., key=Author5, key=Author6. In this example, all authors must be
 *    placed in the same tag.
 * -  Metadata is flat, not hierarchical; there are no subtags. If you
 *    want to store, e.g., the email address of the child of producer Alice
 *    and actor Bob, that could have key=alice_and_bobs_childs_email_address.
 * -  Several modifiers can be applied to the tag name. This is done by
 *    appending a dash character ('-') and the modifier name in the order
 *    they appear in the list below -- e.g. foo-eng-sort, not foo-sort-eng.
 *    -  language -- a tag whose value is localized for a particular language
 *       is appended with the ISO 639-2/B 3-letter language code.
 *       For example: Author-ger=Michael, Author-eng=Mike
 *       The original/default language is in the unqualified "Author" tag.
 *       A demuxer should set a default if it sets any translated tag.
 *    -  sorting  -- a modified version of a tag that should be used for
 *       sorting will have '-sort' appended. E.g. artist="The Beatles",
 *       artist-sort="Beatles, The".
 * - Some protocols and demuxers support metadata updates. After a successful
 *   call to av_read_frame(), AVFormatContext.event_flags or AVStream.event_flags
 *   will be updated to indicate if metadata changed. In order to detect metadata
 *   changes on a stream, you need to loop through all streams in the AVFormatContext
 *   and check their individual event_flags.
 *
 * -  Demuxers attempt to export metadata in a generic format, however tags
 *    with no generic equivalents are left as they are stored in the container.
 *    Follows a list of generic tag names:
 *
 @verbatim
 album        -- name of the set this work belongs to
 album_artist -- main creator of the set/album, if different from artist.
                 e.g. "Various Artists" for compilation albums.
 artist       -- main creator of the work
 comment      -- any additional description of the file.
 composer     -- who composed the work, if different from artist.
 copyright    -- name of copyright holder.
 creation_time-- date when the file was created, preferably in ISO 8601.
 date         -- date when the work was created, preferably in ISO 8601.
 disc         -- number of a subset, e.g. disc in a multi-disc collection.
 encoder      -- name/settings of the software/hardware that produced the file.
 encoded_by   -- person/group who created the file.
 filename     -- original name of the file.
 genre        -- <self-evident>.
 language     -- main language in which the work is performed, preferably
                 in ISO 639-2 format. Multiple languages can be specified by
                 separating them with commas.
 performer    -- artist who performed the work, if different from artist.
                 E.g for "Also sprach Zarathustra", artist would be "Richard
                 Strauss" and performer "London Philharmonic Orchestra".
 publisher    -- name of the label/publisher.
 service_name     -- name of the service in broadcasting (channel name).
 service_provider -- name of the service provider in broadcasting.
 title        -- name of the work.
 track        -- number of this work in the set, can be in form current/total.
 variant_bitrate -- the total bitrate of the bitrate variant that the current stream is part of
 @endverbatim
 *
 * Look in the examples section for an application example how to use the Metadata API.
 *
 * @}
 */
 
/* packet functions */
 
 
/**
 * Allocate and read the payload of a packet and initialize its
 * fields with default values.
 *
 * @param s    associated IO context
 * @param pkt packet
 * @param size desired payload size
 * @return >0 (read size) if OK, AVERROR_xxx otherwise
 */
export function av_get_packet(s: AVIOContext, pkt: AVPacket, size: number): number { throw new Error('NotImplemented'); }
 
 
/**
 * Read data and append it to the current content of the AVPacket.
 * If pkt->size is 0 this is identical to av_get_packet.
 * Note that this uses av_grow_packet and thus involves a realloc
 * which is inefficient. Thus this function should only be used
 * when there is no reasonable way to know (an upper bound of)
 * the final size.
 *
 * @param s    associated IO context
 * @param pkt packet
 * @param size amount of data to read
 * @return >0 (read size) if OK, AVERROR_xxx otherwise, previous data
 *         will not be lost even if an error occurs.
 */
export function av_append_packet(s: AVIOContext, pkt: AVPacket, size: number): number { throw new Error('NotImplemented'); }
 
/*************************************************/
/* input/output formats */
export type AVCodecTag = OpaquePtr;

/**
 * This structure contains the data a format has to probe a file.
 */
export interface AVProbeData {
    filename: string;
    /**< Buffer must have AVPROBE_PADDING_SIZE of extra allocated bytes filled with zero. */
    buf: Buffer;
    /**< Size of buf except extra allocated bytes */
    buf_size: number;
    /**< mime_type, when known. */
    mime_type: string;
}

///< maximum score
export const AVPROBE_SCORE_MAX =       100;

export const AVPROBE_SCORE_RETRY = (AVPROBE_SCORE_MAX/4);
export const AVPROBE_SCORE_STREAM_RETRY = (AVPROBE_SCORE_MAX/4-1);
 
///< score for file extension
export const AVPROBE_SCORE_EXTENSION = 50;

///< score for file mime type
export const AVPROBE_SCORE_MIME =       75;

 
///< extra allocated bytes at the end of the probe buffer
export const AVPROBE_PADDING_SIZE = 32;
 
/// Demuxer will use avio_open, no opened file should be provided by the caller.
export const AVFMT_NOFILE =        0x0001;

/**< Needs '%d' in filename. */
export const AVFMT_NEEDNUMBER =    0x0002;
/**
 * The muxer/demuxer is experimental and should be used with caution.
 *
 * - demuxers: will not be selected automatically by probing, must be specified
 *             explicitly.
 */
export const AVFMT_EXPERIMENTAL =  0x0004;
/**< Show format stream IDs numbers. */
export const AVFMT_SHOW_IDS =      0x0008;

/**< Format wants global header. */
export const AVFMT_GLOBALHEADER =  0x0040;

/**< Format does not need / have any timestamps. */
export const AVFMT_NOTIMESTAMPS =  0x0080;

/**< Use generic index building code. */
export const AVFMT_GENERIC_INDEX = 0x0100;

/**< Format allows timestamp discontinuities. Note, muxers always require valid (monotone) timestamps */
export const AVFMT_TS_DISCONT =    0x0200;

/**< Format allows variable fps. */
export const AVFMT_VARIABLE_FPS =  0x0400;

/**< Format does not need width/height */
export const AVFMT_NODIMENSIONS =  0x0800;

/**< Format does not require any streams */
export const AVFMT_NOSTREAMS =     0x1000;

/**< Format does not allow to fall back on binary search via read_timestamp */
export const AVFMT_NOBINSEARCH =   0x2000;

/**< Format does not allow to fall back on generic search */
export const AVFMT_NOGENSEARCH =   0x4000;

/**< Format does not allow seeking by bytes */
export const AVFMT_NO_BYTE_SEEK =  0x8000;

/**< Format allows flushing. If not set, the muxer will not receive a NULL packet in the write_packet function. */
export const AVFMT_ALLOW_FLUSH =  0x10000;

/**< Format does not require strictly
increasing timestamps, but they must
still be monotonic */
export const AVFMT_TS_NONSTRICT = 0x20000;

/**< Format allows muxing negative
 * timestamps. If not set the timestamp
 * will be shifted in av_write_frame and
 * av_interleaved_write_frame so they
 * start from 0.
 * The user or muxer can override this through
 * AVFormatContext.avoid_negative_ts
 */
export const AVFMT_TS_NEGATIVE =  0x40000;

/**< Seeking is based on PTS */
export const AVFMT_SEEK_TO_PTS =   0x4000000;

/**
 * @addtogroup lavf_encoding
 * @{
 */
export interface AVOutputFormat {
    name: string;
    /**
     * Descriptive name for the format, meant to be more human-readable
     * than name. You should use the NULL_IF_CONFIG_SMALL() macro
     * to define it.
     */
    long_name: string;
    mime_type: string;
    extensions: string; /**< comma-separated filename extensions */
    /* output support */
    /**< default audio codec */
    audio_codec: AVCodecID;

    /**< default video codec */
    video_codec: AVCodecID;

    /**< default subtitle codec */
    subtitle_codec: AVCodecID;

    /**
     * can use flags: AVFMT_NOFILE, AVFMT_NEEDNUMBER,
     * AVFMT_GLOBALHEADER, AVFMT_NOTIMESTAMPS, AVFMT_VARIABLE_FPS,
     * AVFMT_NODIMENSIONS, AVFMT_NOSTREAMS, AVFMT_ALLOW_FLUSH,
     * AVFMT_TS_NONSTRICT, AVFMT_TS_NEGATIVE
     */
    flags: number;
 
    /**
     * List of supported codec_id-codec_tag pairs, ordered by "better
     * choice first". The arrays are all terminated by AV_CODEC_ID_NONE.
     */
    codec_tag: AVCodecTag[];
 
 
    priv_class: AVClass; ///< AVClass for the private context
 
    /*****************************************************************
     * No fields below this line are part of the public API. They
     * may not be used outside of libavformat and can be changed and
     * removed at will.
     * New public fields should be added right above.
     *****************************************************************
     */

    /**
     * size of private data so that it can be allocated in the wrapper
     */
    priv_data_size: number;
 
    /**
     * Internal flags. See FF_FMT_FLAG_* in internal.h.
     */
    flags_internal: number;
 
    write_header: (context: AVFormatContext) => number;

    /**
     * Write a packet. If AVFMT_ALLOW_FLUSH is set in flags,
     * pkt can be NULL in order to flush data buffered in the muxer.
     * When flushing, return 0 if there still is more data to flush,
     * or 1 if everything was flushed and there is no more buffered
     * data.
     */
    write_packet: (context: AVFormatContext, pkg: AVPacket) => number;
    write_trailer: (context: AVFormatContext) => number;

    /**
     * A format-specific function for interleavement.
     * If unset, packets will be interleaved by dts.
     *
     * @param s           An AVFormatContext for output. pkt will be added to
     *                    resp. taken from its packet buffer.
     * @param[in,out] pkt A packet to be interleaved if has_packet is set;
     *                    also used to return packets. If no packet is returned
     *                    (e.g. on error), pkt is blank on return.
     * @param flush       1 if no further packets are available as input and
     *                    all remaining packets should be output.
     * @param has_packet  If set, pkt contains a packet to be interleaved
     *                    on input; otherwise pkt is blank on input.
     * @return 1 if a packet was output, 0 if no packet could be output,
     *         < 0 if an error occurred
     */
    interleave_packet: (context: AVFormatContext, pkg: AVPacket, flush: number, has_packet: number) => number;

    /**
     * Test if the given codec can be stored in this container.
     *
     * @return 1 if the codec is supported, 0 if it is not.
     *         A negative number if unknown.
     *         MKTAG('A', 'P', 'I', 'C') if the codec is only supported as AV_DISPOSITION_ATTACHED_PIC
     */
    query_codec: (id: AVCodecID, std_compliance: number) => number;
 
    get_output_timestamp: (s: AVFormatContext, stream: number, dts: Out<number>, wall: Out<number>) => void;
    /**
     * Allows sending messages from application to device.
     */
    control_message: (s: AVFormatContext, type: number, data: Buffer, data_size: number) => number;
 
    /**
     * Write an uncoded AVFrame.
     *
     * See av_write_uncoded_frame() for details.
     *
     * The library will free *frame afterwards, but the muxer can prevent it
     * by setting the pointer to NULL.
     */
    write_uncoded_frame: (s: AVFormatContext, stream_index: number, frame: AVFrame, flags: number) => number;

    /**
     * Returns device list with it properties.
     * @see avdevice_list_devices() for more details.
     */
    get_device_list: (s: AVFormatContext, device_list: AVDeviceInfoList) => number;

    /**< default data codec */
    data_codec: AVCodecID;

    /**
     * Initialize format. May allocate data here, and set any AVFormatContext or
     * AVStream parameters that need to be set before packets are sent.
     * This method must not write output.
     *
     * Return 0 if streams were fully configured, 1 if not, negative AVERROR on failure
     *
     * Any allocations made here must be freed in deinit().
     */
    init: (s: AVFormatContext) => number;

    /**
     * Deinitialize format. If present, this is called whenever the muxer is being
     * destroyed, regardless of whether or not the header has been written.
     *
     * If a trailer is being written, this is called after write_trailer().
     *
     * This is called if init() fails as well.
     */
    deinit: (s: AVFormatContext) => number;

    /**
     * Set up any necessary bitstream filtering and extract any extra data needed
     * for the global header.
     *
     * @note pkt might have been directly forwarded by a meta-muxer; therefore
     *       pkt->stream_index as well as the pkt's timebase might be invalid.
     * Return 0 if more packets from this stream must be checked; 1 if not.
     */
    check_bitstream: (s: AVFormatContext, st: AVStreamParseType, pkg: AVPacket) => number;
}

/**
 * @}
 */
 
/**
 * @addtogroup lavf_decoding
 * @{
 */
export interface AVInputFormat {
    /**
     * A comma separated list of short names for the format. New names
     * may be appended with a minor bump.
     */
    name: string;
 
    /**
     * Descriptive name for the format, meant to be more human-readable
     * than name. You should use the NULL_IF_CONFIG_SMALL() macro
     * to define it.
     */
    long_name: string;
 
    /**
     * Can use flags: AVFMT_NOFILE, AVFMT_NEEDNUMBER, AVFMT_SHOW_IDS,
     * AVFMT_NOTIMESTAMPS, AVFMT_GENERIC_INDEX, AVFMT_TS_DISCONT, AVFMT_NOBINSEARCH,
     * AVFMT_NOGENSEARCH, AVFMT_NO_BYTE_SEEK, AVFMT_SEEK_TO_PTS.
     */
    flags: number;
 
    /**
     * If extensions are defined, then no probe is done. You should
     * usually not use extension format guessing because it is not
     * reliable enough
     */
    extensions: string;
 
    codec_tag: AVCodecTag[];
 
    ///< AVClass for the private context
    priv_class: AVClass; 
 
    /**
     * Comma-separated list of mime types.
     * It is used check for matching mime types while probing.
     * @see av_probe_input_format2
     */
    mime_type: string;
 
    /*****************************************************************
     * No fields below this line are part of the public API. They
     * may not be used outside of libavformat and can be changed and
     * removed at will.
     * New public fields should be added right above.
     *****************************************************************
     */
    /**
     * Raw demuxers store their codec ID here.
     */
    raw_codec_id: number;
 
    /**
     * Size of private data so that it can be allocated in the wrapper.
     */
    priv_data_size: number;
 
    /**
     * Internal flags. See FF_FMT_FLAG_* in internal.h.
     */
    flags_internal: number;
 
    /**
     * Tell if a given file has a chance of being parsed as this format.
     * The buffer provided is guaranteed to be AVPROBE_PADDING_SIZE bytes
     * big so you do not have to check for that unless you need more.
     */
    read_probe: (data: AVProbeData) => number;
 
    /**
     * Read the format header and initialize the AVFormatContext
     * structure. Return 0 if OK. 'avformat_new_stream' should be
     * called to create new streams.
     */
    read_header: (s: AVFormatContext) => number;
 
    /**
     * Read one packet and put it in 'pkt'. pts and flags are also
     * set. 'avformat_new_stream' can be called only if the flag
     * AVFMTCTX_NOHEADER is used and only in the calling thread (not in a
     * background thread).
     * @return 0 on success, < 0 on error.
     *         Upon returning an error, pkt must be unreferenced by the caller.
     */
    read_packet: (s: AVFormatContext, pkg: AVPacket) => number;
 
    /**
     * Close the stream. The AVFormatContext and AVStreams are not
     * freed by this function
     */
    read_close: (s: AVFormatContext) => number;
 
    /**
     * Seek to a given timestamp relative to the frames in
     * stream component stream_index.
     * @param stream_index Must not be -1.
     * @param flags Selects which direction should be preferred if no exact
     *              match is available.
     * @return >= 0 on success (but not necessarily the new offset)
     */
    read_seek: (s: AVFormatContext, stream_index: number, timestamp: number, flags: number) => number;
 
    /**
     * Get the next timestamp in stream[stream_index].time_base units.
     * @return the timestamp or AV_NOPTS_VALUE if an error occurred
     */
    read_timestamp: (s: AVFormatContext, stream_index: number, pos: Out<number>, pos_limit: number) => number;
 
    /**
     * Start/resume playing - only meaningful if using a network-based format
     * (RTSP).
     */
    read_play: (s: AVFormatContext) => number;
 
    /**
     * Pause playing - only meaningful if using a network-based format
     * (RTSP).
     */
    read_pause: (s: AVFormatContext) => number;
 
    /**
     * Seek to timestamp ts.
     * Seeking will be done so that the point from which all active streams
     * can be presented successfully will be closest to ts and within min/max_ts.
     * Active streams are all streams that have AVStream.discard < AVDISCARD_ALL.
     */
    read_seek2: (s: AVFormatContext, stream_index: number, min_ts: number, ts: number, max_ts: number, flags: number) => number;
 
    /**
     * Returns device list with it properties.
     * @see avdevice_list_devices() for more details.
     */
    get_device_list: (s: AVFormatContext, device_list: Out<AVDeviceInfoList>) => number;
 
}

/**
 * @}
 */
enum AVStreamParseType {
    AVSTREAM_PARSE_NONE,

    /**< full parsing and repack */
    AVSTREAM_PARSE_FULL,

    /**< Only parse headers, do not repack. */
    AVSTREAM_PARSE_HEADERS,

    /**< full parsing and interpolation of timestamps for frames not starting on a packet boundary */
    AVSTREAM_PARSE_TIMESTAMPS,

    /**< full parsing and repack of the first frame only, only implemented for H.264 currently */
    AVSTREAM_PARSE_FULL_ONCE,

    /**< full parsing and repack with timestamp and position generation by parser for raw
        this assumes that each packet in the file contains no demuxer level headers and
        just codec level data, otherwise position generation would fail */
    AVSTREAM_PARSE_FULL_RAW,
}

export const AVINDEX_KEYFRAME = 0x0001;
/**
 * Flag is used to indicate which frame should be discarded after decoding.
 */
export const AVINDEX_DISCARD_FRAME = 0x0002;

export interface AVIndexEntry {
    pos: number;
    /**<
     * Timestamp in AVStream.time_base units, preferably the time from which on correctly decoded frames are available
     * when seeking to this entry. That means preferable PTS on keyframe based formats.
     * But demuxers can choose to store a different timestamp, if it is more convenient for the implementation or nothing better
     * is known
     */
    timestamp: number;


    /**
     * @bitpack 2
     */
    flags: number;

    /**
     * @bitpack 30
     * @note Yeah, trying to keep the size of this small to reduce memory requirements (it is 24 vs. 32 bytes due to possible 8-byte alignment).
     */
    size: number;

    /**< Minimum distance between this and the previous keyframe, used to avoid unneeded searching. */
    min_distance: number;
}
 
/**
 * The stream should be chosen by default among other streams of the same type,
 * unless the user has explicitly specified otherwise.
 */
export const AV_DISPOSITION_DEFAULT =              (1 << 0);
/**
 * The stream is not in original language.
 *
 * @note AV_DISPOSITION_ORIGINAL is the inverse of this disposition. At most
 *       one of them should be set in properly tagged streams.
 * @note This disposition may apply to any stream type, not just audio.
 */
export const AV_DISPOSITION_DUB =                  (1 << 1);
/**
 * The stream is in original language.
 *
 * @see the notes for AV_DISPOSITION_DUB
 */
export const AV_DISPOSITION_ORIGINAL =             (1 << 2);
/**
 * The stream is a commentary track.
 */
export const AV_DISPOSITION_COMMENT =              (1 << 3);
/**
 * The stream contains song lyrics.
 */
export const AV_DISPOSITION_LYRICS =               (1 << 4);
/**
 * The stream contains karaoke audio.
 */
export const AV_DISPOSITION_KARAOKE =              (1 << 5);
 
/**
 * Track should be used during playback by default.
 * Useful for subtitle track that should be displayed
 * even when user did not explicitly ask for subtitles.
 */
export const AV_DISPOSITION_FORCED =               (1 << 6);
/**
 * The stream is intended for hearing impaired audiences.
 */
export const AV_DISPOSITION_HEARING_IMPAIRED =     (1 << 7);
/**
 * The stream is intended for visually impaired audiences.
 */
export const AV_DISPOSITION_VISUAL_IMPAIRED =      (1 << 8);
/**
 * The audio stream contains music and sound effects without voice.
 */
export const AV_DISPOSITION_CLEAN_EFFECTS =        (1 << 9);
/**
 * The stream is stored in the file as an attached picture/"cover art" (e.g.
 * APIC frame in ID3v2). The first (usually only) packet associated with it
 * will be returned among the first few packets read from the file unless
 * seeking takes place. It can also be accessed at any time in
 * AVStream.attached_pic.
 */
export const AV_DISPOSITION_ATTACHED_PIC =         (1 << 10);
/**
 * The stream is sparse, and contains thumbnail images, often corresponding
 * to chapter markers. Only ever used with AV_DISPOSITION_ATTACHED_PIC.
 */
export const AV_DISPOSITION_TIMED_THUMBNAILS =     (1 << 11);
 
/**
 * The stream is intended to be mixed with a spatial audio track. For example,
 * it could be used for narration or stereo music, and may remain unchanged by
 * listener head rotation.
 */
export const AV_DISPOSITION_NON_DIEGETIC =         (1 << 12);
 
/**
 * The subtitle stream contains captions, providing a transcription and possibly
 * a translation of audio. Typically intended for hearing-impaired audiences.
 */
export const AV_DISPOSITION_CAPTIONS =             (1 << 16);
/**
 * The subtitle stream contains a textual description of the video content.
 * Typically intended for visually-impaired audiences or for the cases where the
 * video cannot be seen.
 */
export const AV_DISPOSITION_DESCRIPTIONS =         (1 << 17);
/**
 * The subtitle stream contains time-aligned metadata that is not intended to be
 * directly presented to the user.
 */
export const AV_DISPOSITION_METADATA =             (1 << 18);
/**
 * The audio stream is intended to be mixed with another stream before
 * presentation.
 * Corresponds to mix_type=0 in mpegts.
 */
export const AV_DISPOSITION_DEPENDENT =            (1 << 19);
/**
 * The video stream contains still images.
 */
export const AV_DISPOSITION_STILL_IMAGE =          (1 << 20);
 
/**
 * @return The AV_DISPOSITION_* flag corresponding to disp or a negative error
 *         code if disp does not correspond to a known stream disposition.
 */
export function av_disposition_from_string(disp: string): number { throw new Error('NotImplemented'); }
 
/**
 * @param disposition a combination of AV_DISPOSITION_* values
 * @return The string description corresponding to the lowest set bit in
 *         disposition. NULL when the lowest set bit does not correspond
 *         to a known disposition or when disposition is 0.
 */
export function av_disposition_to_string(disposition: number): string { throw new Error('NotImplemented'); }
 
/**
 * Options for behavior on timestamp wrap detection.
 */

///< ignore the wrap
export const AV_PTS_WRAP_IGNORE =      0;

///< add the format specific offset on wrap detection
export const AV_PTS_WRAP_ADD_OFFSET =  1;

///< subtract the format specific offset on wrap detection
export const AV_PTS_WRAP_SUB_OFFSET =  -1;
/**
 * - demuxing: the demuxer read new metadata from the file and updated
 *     AVStream.metadata accordingly
 * - muxing: the user updated AVStream.metadata and wishes the muxer to write
 *     it into the file
 */
export const AVSTREAM_EVENT_FLAG_METADATA_UPDATED = 0x0001;

 /**
  * - demuxing: new packets for this stream were read from the file. This
  *   event is informational only and does not guarantee that new packets
  *   for this stream will necessarily be returned from av_read_frame().
  */
export const AVSTREAM_EVENT_FLAG_NEW_PACKETS = (1 << 1);
 
/**
 * Stream structure.
 * New fields can be added to the end with minor version bumps.
 * Removal, reordering and changes to existing fields require a major
 * version bump.
 * sizeof(AVStream) must not be used outside libav*.
 */
export interface AVStream {
    /**
     * A class for @ref avoptions. Set on stream creation.
     * @if FF_API_AVSTREAM_CLASS
     */
    av_class: AVClass;
 
    /**< stream index in AVFormatContext */
    index: number;

    /**
     * Format-specific stream ID.
     * decoding: set by libavformat
     * encoding: set by the user, replaced by libavformat if left unset
     */
    id: number;
 
    priv_data: Buffer;
 
    /**
     * This is the fundamental unit of time (in seconds) in terms
     * of which frame timestamps are represented.
     *
     * decoding: set by libavformat
     * encoding: May be set by the caller before avformat_write_header() to
     *           provide a hint to the muxer about the desired timebase. In
     *           avformat_write_header(), the muxer will overwrite this field
     *           with the timebase that will actually be used for the timestamps
     *           written into the file (which may or may not be related to the
     *           user-provided one, depending on the format).
     */
    time_base: AVRational;
 
    /**
     * Decoding: pts of the first frame of the stream in presentation order, in stream time base.
     * Only set this if you are absolutely 100% sure that the value you set
     * it to really is the pts of the first frame.
     * This may be undefined (AV_NOPTS_VALUE).
     * @note The ASF header does NOT contain a correct start_time the ASF
     * demuxer must NOT set this.
     */
    start_time: number;
 
    /**
     * Decoding: duration of the stream, in stream time base.
     * If a source file does not specify a duration, but does specify
     * a bitrate, this value will be estimated from bitrate and file size.
     *
     * Encoding: May be set by the caller before avformat_write_header() to
     * provide a hint to the muxer about the estimated duration.
     */
    duration: number;
 
    ///< number of frames in this stream if known or 0
    nb_frames: number;
 
    /**
     * Stream disposition - a combination of AV_DISPOSITION_* flags.
     * - demuxing: set by libavformat when creating the stream or in
     *             avformat_find_stream_info().
     * - muxing: may be set by the caller before avformat_write_header().
     */
    disposition: number;
 
    ///< Selects which packets can be discarded at will and do not need to be demuxed.
    discard: AVDiscard;
 
    /**
     * sample aspect ratio (0 if unknown)
     * - encoding: Set by user.
     * - decoding: Set by libavformat.
     */
    sample_aspect_ratio: AVRational;
 
    metadata: AVDictionary;
 
    /**
     * Average framerate
     *
     * - demuxing: May be set by libavformat when creating the stream or in
     *             avformat_find_stream_info().
     * - muxing: May be set by the caller before avformat_write_header().
     */
    avg_frame_rate: AVRational;
 
    /**
     * For streams with AV_DISPOSITION_ATTACHED_PIC disposition, this packet
     * will contain the attached picture.
     *
     * decoding: set by libavformat, must not be modified by the caller.
     * encoding: unused
     */
    attached_pic: AVPacket;
 
    /**
     * An array of side data that applies to the whole stream (i.e. the
     * container does not allow it to change between packets).
     *
     * There may be no overlap between the side data in this array and side data
     * in the packets. I.e. a given side data is either exported by the muxer
     * (demuxing) / set by the caller (muxing) in this array, then it never
     * appears in the packets, or the side data is exported / sent through
     * the packets (always in the first packet where the value becomes known or
     * changes), then it does not appear in this array.
     *
     * - demuxing: Set by libavformat when the stream is created.
     * - muxing: May be set by the caller before avformat_write_header().
     *
     * Freed by libavformat in avformat_free_context().
     *
     * @see av_format_inject_global_side_data()
     */
    side_data: AVPacketSideData;
    /**
     * The number of elements in the AVStream.side_data array.
     */
    nb_side_data: number;
 
    /**
     * Flags indicating events happening on the stream, a combination of
     * AVSTREAM_EVENT_FLAG_*.
     *
     * - demuxing: may be set by the demuxer in avformat_open_input(),
     *   avformat_find_stream_info() and av_read_frame(). Flags must be cleared
     *   by the user once the event has been handled.
     * - muxing: may be set by the user after avformat_write_header(). to
     *   indicate a user-triggered event.  The muxer will clear the flags for
     *   events it has handled in av_[interleaved]_write_frame().
     */
    event_flags: number;

    /**
     * Real base framerate of the stream.
     * This is the lowest framerate with which all timestamps can be
     * represented accurately (it is the least common multiple of all
     * framerates in the stream). Note, this value is just a guess!
     * For example, if the time base is 1/90000 and all frames have either
     * approximately 3600 or 1800 timer ticks, then r_frame_rate will be 50/1.
     */
    r_frame_rate: AVRational;
 
    /**
     * Codec parameters associated with this stream. Allocated and freed by
     * libavformat in avformat_new_stream() and avformat_free_context()
     * respectively.
     *
     * - demuxing: filled by libavformat on stream creation or in
     *             avformat_find_stream_info()
     * - muxing: filled by the caller before avformat_write_header()
     */
    codecpar: AVCodecParameters;
 
    /**
     * Number of bits in timestamps. Used for wrapping control.
     *
     * - demuxing: set by libavformat
     * - muxing: set by libavformat
     *
     */
    pts_wrap_bits: number;
}
 
export function av_stream_get_parser(s: AVStream): AVCodecParserContext { throw new Error('NotImplemented'); }
 
/**
 * Returns the pts of the last muxed packet + its duration
 *
 * the retuned value is undefined when used with a demuxer.
 */
export function av_stream_get_end_pts(st: AVStream): number { throw new Error('NotImplemented'); }
 
export const AV_PROGRAM_RUNNING = 1;
 
/**
 * New fields can be added to the end with minor version bumps.
 * Removal, reordering and changes to existing fields require a major
 * version bump.
 * sizeof(AVProgram) must not be used outside libav*.
 */
export interface AVProgram {
    id: number;
    flags: number;

    ///< selects which program to discard and which to feed to the caller
    discard: AVDiscard;

    /**
     * @nodeTODO unsigned int* ?
     */
    stream_index: number;
    nb_stream_indexes: number;
    metadata: AVDictionary;
 
    program_num: number;
    pmt_pid: number;
    pcr_pid: number;
    pmt_version: number;
 
    /*****************************************************************
     * All fields below this line are not part of the public API. They
     * may not be used outside of libavformat and can be changed and
     * removed at will.
     * New public fields should be added right above.
     *****************************************************************
     */
    start_time: number;
    end_time: number;
 
    ///< reference dts for wrap detection
    pts_wrap_reference: number;

    ///< behavior on wrap detection
    pts_wrap_behavior: number;
}
 
/**< signal that no header is present
     (streams are added dynamically) */
export const AVFMTCTX_NOHEADER =      0x0001;

/**< signal that the stream is definitely
    not seekable, and attempts to call the
    seek function will fail. For some
    network protocols (e.g. HLS), this can
    change dynamically at runtime. */
export const AVFMTCTX_UNSEEKABLE =    0x0002; 
 
export interface AVChapter {
    ///< unique ID to identify the chapter
    id: number;

    ///< time base in which the start/end timestamps are specified
    time_base: AVRational;

    ///< chapter start time in time_base units
    start: number;
    
    ///< chapter end time in time_base units
    end: number;

    metadata: AVDictionary;
}

/**
 * Callback used by devices to communicate with application.
 */
type av_format_control_message = (s: AVFormatContext, type: number, data: Buffer, data_size: number) => number;
type AVOpenCallback = (s: AVFormatContext, pb: Out<AVIOContext>, url: string, flags: number, int_cb: 
    AVIOInterruptCB, options: AVDictionary) => number;
 
/**
 * The duration of a video can be estimated through various ways, and this enum can be used
 * to know how the duration was estimated.
 */
enum AVDurationEstimationMethod {
    AVFMT_DURATION_FROM_PTS,    ///< Duration accurately estimated from PTSes
    AVFMT_DURATION_FROM_STREAM, ///< Duration estimated from a stream with a known duration
    AVFMT_DURATION_FROM_BITRATE ///< Duration estimated from bitrate (less accurate)
}

///< Generate missing pts even if it requires parsing future frames.
export const AVFMT_FLAG_GENPTS =       0x0001;

///< Ignore index.
export const AVFMT_FLAG_IGNIDX =       0x0002;

///< Do not block when reading packets from input.
export const AVFMT_FLAG_NONBLOCK =     0x0004;

///< Ignore DTS on frames that contain both DTS & PTS
export const AVFMT_FLAG_IGNDTS =       0x0008;

///< Do not infer any values from other values, just return what is stored in the container
export const AVFMT_FLAG_NOFILLIN =     0x0010;

///< Do not use AVParsers, you also must set AVFMT_FLAG_NOFILLIN as the fillin code works on frames and no parsing -> no frames. Also seeking to frames can not work if parsing to find frame boundaries has been disabled
export const AVFMT_FLAG_NOPARSE =      0x0020;

///< Do not buffer frames when possible
export const AVFMT_FLAG_NOBUFFER =     0x0040;

///< The caller has supplied a custom AVIOContext, don't avio_close() it.
export const AVFMT_FLAG_CUSTOM_IO =    0x0080;

///< Discard frames marked corrupted
export const AVFMT_FLAG_DISCARD_CORRUPT =  0x0100;

///< Flush the AVIOContext every packet.
export const AVFMT_FLAG_FLUSH_PACKETS =    0x0200;

/**
 * When muxing, try to avoid writing any random/volatile data to the output.
 * This includes any random IDs, real-time timestamps/dates, muxer version, etc.
 *
 * This flag is mainly intended for testing.
 */
export const AVFMT_FLAG_BITEXACT =         0x0400;
///< try to interleave outputted packets by dts (using this flag can slow demuxing down)
export const AVFMT_FLAG_SORT_DTS =    0x10000;


/**
 * Enable use of private options by delaying codec open (deprecated, does nothing)
 * @if FF_API_LAVF_PRIV_OPT
 */
export const AVFMT_FLAG_PRIV_OPT =    0x20000;

///< Enable fast, but inaccurate seeks for some formats
export const AVFMT_FLAG_FAST_SEEK =   0x80000;

///< Stop muxing when the shortest stream stops.
export const AVFMT_FLAG_SHORTEST =   0x100000;

///< Add bitstream filters as requested by the muxer
export const AVFMT_FLAG_AUTO_BSF =   0x200000;

export const FF_FDEBUG_TS =        0x0001;

/**
 * - demuxing: the demuxer read new metadata from the file and updated
 *   AVFormatContext.metadata accordingly
 * - muxing: the user updated AVFormatContext.metadata and wishes the muxer to
 *   write it into the file
 */
export const AVFMT_EVENT_FLAG_METADATA_UPDATED = 0x0001;

///< Enabled when required by target format
export const AVFMT_AVOID_NEG_TS_AUTO =             -1;

///< Do not shift timestamps even when they are negative.
export const AVFMT_AVOID_NEG_TS_DISABLED =          0;

///< Shift timestamps so they are non negative
export const AVFMT_AVOID_NEG_TS_MAKE_NON_NEGATIVE = 1;

///< Shift timestamps so that they start at 0
export const AVFMT_AVOID_NEG_TS_MAKE_ZERO =         2;


/**
 * Format I/O context.
 * New fields can be added to the end with minor version bumps.
 * Removal, reordering and changes to existing fields require a major
 * version bump.
 * sizeof(AVFormatContext) must not be used outside libav*, use
 * avformat_alloc_context() to create an AVFormatContext.
 *
 * Fields can be accessed through AVOptions (av_opt*),
 * the name string used matches the associated command line parameter name and
 * can be found in libavformat/options_table.h.
 * The AVOption/command line parameter names differ in some cases from the C
 * structure field names for historic reasons or brevity.
 */
export interface AVFormatContext {
    /**
     * A class for logging and @ref avoptions. Set by avformat_alloc_context().
     * Exports (de)muxer private options if they exist.
     */
    av_class: AVClass;
 
    /**
     * The input container format.
     *
     * Demuxing only, set by avformat_open_input().
     */
    iformat: AVInputFormat;
 
    /**
     * The output container format.
     *
     * Muxing only, must be set by the caller before avformat_write_header().
     */
    oformat: AVOutputFormat;
 
    /**
     * Format private data. This is an AVOptions-enabled struct
     * if and only if iformat/oformat.priv_class is not NULL.
     *
     * - muxing: set by avformat_write_header()
     * - demuxing: set by avformat_open_input()
     */
    priv_data: Buffer;
 
    /**
     * I/O context.
     *
     * - demuxing: either set by the user before avformat_open_input() (then
     *             the user must close it manually) or set by avformat_open_input().
     * - muxing: set by the user before avformat_write_header(). The caller must
     *           take care of closing / freeing the IO context.
     *
     * Do NOT set this field if AVFMT_NOFILE flag is set in
     * iformat/oformat.flags. In such a case, the (de)muxer will handle
     * I/O in some other way and this field will be NULL.
     */
    pb: AVIOContext;
 
    /* stream info */
    /**
     * Flags signalling stream properties. A combination of AVFMTCTX_*.
     * Set by libavformat.
     */
    ctx_flags: number;
 
    /**
     * Number of elements in AVFormatContext.streams.
     *
     * Set by avformat_new_stream(), must not be modified by any other code.
     */
    nb_streams: number;
    /**
     * A list of all streams in the file. New streams are created with
     * avformat_new_stream().
     *
     * - demuxing: streams are created by libavformat in avformat_open_input().
     *             If AVFMTCTX_NOHEADER is set in ctx_flags, then new streams may also
     *             appear in av_read_frame().
     * - muxing: streams are created by the user before avformat_write_header().
     *
     * Freed by libavformat in avformat_free_context().
     */
    streams: AVStream[];
 
    /**
     * input or output URL. Unlike the old filename field, this field has no
     * length restriction.
     *
     * - demuxing: set by avformat_open_input(), initialized to an empty
     *             string if url parameter was NULL in avformat_open_input().
     * - muxing: may be set by the caller before calling avformat_write_header()
     *           (or avformat_init_output() if that is called first) to a string
     *           which is freeable by av_free(). Set to an empty string if it
     *           was NULL in avformat_init_output().
     *
     * Freed by libavformat in avformat_free_context().
     */
    url: string;
 
    /**
     * Position of the first frame of the component, in
     * AV_TIME_BASE fractional seconds. NEVER set this value directly:
     * It is deduced from the AVStream values.
     *
     * Demuxing only, set by libavformat.
     */
    start_time: number;
 
    /**
     * Duration of the stream, in AV_TIME_BASE fractional
     * seconds. Only set this value if you know none of the individual stream
     * durations and also do not set any of them. This is deduced from the
     * AVStream values if not set.
     *
     * Demuxing only, set by libavformat.
     */
    duration: number;
 
    /**
     * Total stream bitrate in bit/s, 0 if not
     * available. Never set it directly if the file_size and the
     * duration are known as FFmpeg can compute it automatically.
     */
    bit_rate: number;
 
    packet_size: number;
    max_delay: number;
 
    /**
     * Flags modifying the (de)muxer behaviour. A combination of AVFMT_FLAG_*.
     * Set by the user before avformat_open_input() / avformat_write_header().
     */
    flags: number;
 
    /**
     * Maximum number of bytes read from input in order to determine stream
     * properties. Used when reading the global header and in
     * avformat_find_stream_info().
     *
     * Demuxing only, set by the caller before avformat_open_input().
     *
     * @note this is \e not  used for determining the \ref AVInputFormat
     *       "input format"
     * @sa format_probesize
     */
    probesize: number;
 
    /**
     * Maximum duration (in AV_TIME_BASE units) of the data read
     * from input in avformat_find_stream_info().
     * Demuxing only, set by the caller before avformat_find_stream_info().
     * Can be set to 0 to let avformat choose using a heuristic.
     */
    max_analyze_duration: number;
 
    key: Buffer;
    keylen: number;
 
    nb_programs: number;
    programs: AVProgram[];
 
    /**
     * Forced video codec_id.
     * Demuxing: Set by user.
     */
    video_codec_id: AVCodecID;
 
    /**
     * Forced audio codec_id.
     * Demuxing: Set by user.
     */
    audio_codec_id: AVCodecID;
 
    /**
     * Forced subtitle codec_id.
     * Demuxing: Set by user.
     */
    subtitle_codec_id: AVCodecID;
 
    /**
     * Maximum amount of memory in bytes to use for the index of each stream.
     * If the index exceeds this size, entries will be discarded as
     * needed to maintain a smaller size. This can lead to slower or less
     * accurate seeking (depends on demuxer).
     * Demuxers for which a full in-memory index is mandatory will ignore
     * this.
     * - muxing: unused
     * - demuxing: set by user
     */
    max_index_size: number;
 
    /**
     * Maximum amount of memory in bytes to use for buffering frames
     * obtained from realtime capture devices.
     */
    max_picture_buffer: number;
 
    /**
     * Number of chapters in AVChapter array.
     * When muxing, chapters are normally written in the file header,
     * so nb_chapters should normally be initialized before write_header
     * is called. Some muxers (e.g. mov and mkv) can also write chapters
     * in the trailer.  To write chapters in the trailer, nb_chapters
     * must be zero when write_header is called and non-zero when
     * write_trailer is called.
     * - muxing: set by user
     * - demuxing: set by libavformat
     */
    nb_chapters: number;
    chapters: AVChapter[];
 
    /**
     * Metadata that applies to the whole file.
     *
     * - demuxing: set by libavformat in avformat_open_input()
     * - muxing: may be set by the caller before avformat_write_header()
     *
     * Freed by libavformat in avformat_free_context().
     */
    metadata: AVDictionary;
 
    /**
     * Start time of the stream in real world time, in microseconds
     * since the Unix epoch (00:00 1st January 1970). That is, pts=0 in the
     * stream was captured at this real world time.
     * - muxing: Set by the caller before avformat_write_header(). If set to
     *           either 0 or AV_NOPTS_VALUE, then the current wall-time will
     *           be used.
     * - demuxing: Set by libavformat. AV_NOPTS_VALUE if unknown. Note that
     *             the value may become known after some number of frames
     *             have been received.
     */
    start_time_realtime: number;
 
    /**
     * The number of frames used for determining the framerate in
     * avformat_find_stream_info().
     * Demuxing only, set by the caller before avformat_find_stream_info().
     */
    fps_probe_size: number;
 
    /**
     * Error recognition; higher values will detect more errors but may
     * misdetect some more or less valid parts as errors.
     * Demuxing only, set by the caller before avformat_open_input().
     */
    error_recognition: number;
 
    /**
     * Custom interrupt callbacks for the I/O layer.
     *
     * demuxing: set by the user before avformat_open_input().
     * muxing: set by the user before avformat_write_header()
     * (mainly useful for AVFMT_NOFILE formats). The callback
     * should also be passed to avio_open2() if it's used to
     * open the file.
     */
    interrupt_callback: AVIOInterruptCB;
 
    /**
     * Flags to enable debugging.
     */
    debug: number;
 
    /**
     * Maximum buffering duration for interleaving.
     *
     * To ensure all the streams are interleaved correctly,
     * av_interleaved_write_frame() will wait until it has at least one packet
     * for each stream before actually writing any packets to the output file.
     * When some streams are "sparse" (i.e. there are large gaps between
     * successive packets), this can result in excessive buffering.
     *
     * This field specifies the maximum difference between the timestamps of the
     * first and the last packet in the muxing queue, above which libavformat
     * will output a packet regardless of whether it has queued a packet for all
     * the streams.
     *
     * Muxing only, set by the caller before avformat_write_header().
     */
    max_interleave_delta: number;
 
    /**
     * Allow non-standard and experimental extension
     * @see AVCodecContext.strict_std_compliance
     */
    strict_std_compliance: number;
 
    /**
     * Flags indicating events happening on the file, a combination of
     * AVFMT_EVENT_FLAG_*.
     *
     * - demuxing: may be set by the demuxer in avformat_open_input(),
     *   avformat_find_stream_info() and av_read_frame(). Flags must be cleared
     *   by the user once the event has been handled.
     * - muxing: may be set by the user after avformat_write_header() to
     *   indicate a user-triggered event.  The muxer will clear the flags for
     *   events it has handled in av_[interleaved]_write_frame().
     */
    event_flags: number;
 
    /**
     * Maximum number of packets to read while waiting for the first timestamp.
     * Decoding only.
     */
    max_ts_probe: number;
 
    /**
     * Avoid negative timestamps during muxing.
     * Any value of the AVFMT_AVOID_NEG_TS_* constants.
     * Note, this works better when using av_interleaved_write_frame().
     * - muxing: Set by user
     * - demuxing: unused
     */
    avoid_negative_ts: number;
 
    /**
     * Transport stream id.
     * This will be moved into demuxer private options. Thus no API/ABI compatibility
     */
    ts_id: number;
 
    /**
     * Audio preload in microseconds.
     * Note, not all formats support this and unpredictable things may happen if it is used when not supported.
     * - encoding: Set by user
     * - decoding: unused
     */
    audio_preload: number;
 
    /**
     * Max chunk time in microseconds.
     * Note, not all formats support this and unpredictable things may happen if it is used when not supported.
     * - encoding: Set by user
     * - decoding: unused
     */
    max_chunk_duration: number;
 
    /**
     * Max chunk size in bytes
     * Note, not all formats support this and unpredictable things may happen if it is used when not supported.
     * - encoding: Set by user
     * - decoding: unused
     */
    max_chunk_size: number;
 
    /**
     * forces the use of wallclock timestamps as pts/dts of packets
     * This has undefined results in the presence of B frames.
     * - encoding: unused
     * - decoding: Set by user
     */
    use_wallclock_as_timestamps: number;
 
    /**
     * avio flags, used to force AVIO_FLAG_DIRECT.
     * - encoding: unused
     * - decoding: Set by user
     */
    avio_flags: number;
 
    /**
     * The duration field can be estimated through various ways, and this field can be used
     * to know how the duration was estimated.
     * - encoding: unused
     * - decoding: Read by user
     */
    duration_estimation_method: AVDurationEstimationMethod;
 
    /**
     * Skip initial bytes when opening stream
     * - encoding: unused
     * - decoding: Set by user
     */
    skip_initial_bytes: number;
 
    /**
     * Correct single timestamp overflows
     * - encoding: unused
     * - decoding: Set by user
     */
    correct_ts_overflow: number;
 
    /**
     * Force seeking to any (also non key) frames.
     * - encoding: unused
     * - decoding: Set by user
     */
    seek2any: number;
 
    /**
     * Flush the I/O context after each packet.
     * - encoding: Set by user
     * - decoding: unused
     */
    flush_packets: number;
 
    /**
     * format probing score.
     * The maximal score is AVPROBE_SCORE_MAX, its set when the demuxer probes
     * the format.
     * - encoding: unused
     * - decoding: set by avformat, read by user
     */
    probe_score: number;
 
    /**
     * Maximum number of bytes read from input in order to identify the
     * \ref AVInputFormat "input format". Only used when the format is not set
     * explicitly by the caller.
     *
     * Demuxing only, set by the caller before avformat_open_input().
     *
     * @sa probesize
     */
    format_probesize: number;
 
    /**
     * ',' separated list of allowed decoders.
     * If NULL then all are allowed
     * - encoding: unused
     * - decoding: set by user
     */
    codec_whitelist: string;
 
    /**
     * ',' separated list of allowed demuxers.
     * If NULL then all are allowed
     * - encoding: unused
     * - decoding: set by user
     */
    format_whitelist: string;
 
    /**
     * IO repositioned flag.
     * This is set by avformat when the underlaying IO context read pointer
     * is repositioned, for example when doing byte based seeking.
     * Demuxers can use the flag to detect such changes.
     */
    io_repositioned: number;
 
    /**
     * Forced video codec.
     * This allows forcing a specific decoder, even when there are multiple with
     * the same codec_id.
     * Demuxing: Set by user
     */
    video_codec: AVCodec;
 
    /**
     * Forced audio codec.
     * This allows forcing a specific decoder, even when there are multiple with
     * the same codec_id.
     * Demuxing: Set by user
     */
    audio_codec: AVCodec;
 
    /**
     * Forced subtitle codec.
     * This allows forcing a specific decoder, even when there are multiple with
     * the same codec_id.
     * Demuxing: Set by user
     */
    subtitle_codec: AVCodec;
 
    /**
     * Forced data codec.
     * This allows forcing a specific decoder, even when there are multiple with
     * the same codec_id.
     * Demuxing: Set by user
     */
    data_codec: AVCodec;
 
    /**
     * Number of bytes to be written as padding in a metadata header.
     * Demuxing: Unused.
     * Muxing: Set by user via av_format_set_metadata_header_padding.
     */
    metadata_header_padding: number;
 
    /**
     * User data.
     * This is a place for some private data of the user.
     */
    opaque: Buffer;
 
    /**
     * Callback used by devices to communicate with application.
     */
    control_message_cb: av_format_control_message;
 
    /**
     * Output timestamp offset, in microseconds.
     * Muxing: set by user
     */
    output_ts_offset: number;
 
    /**
     * dump format separator.
     * can be ", " or "\n      " or anything else
     * - muxing: Set by user.
     * - demuxing: Set by user.
     */
    dump_separator: string;
 
    /**
     * Forced Data codec_id.
     * Demuxing: Set by user.
     */
    data_codec_id: AVCodecID;
 
    /**
     * ',' separated list of allowed protocols.
     * - encoding: unused
     * - decoding: set by user
     */
    protocol_whitelist: string;
 
    /**
     * A callback for opening new IO streams.
     *
     * Whenever a muxer or a demuxer needs to open an IO stream (typically from
     * avformat_open_input() for demuxers, but for certain formats can happen at
     * other times as well), it will call this callback to obtain an IO context.
     *
     * @param s the format context
     * @param pb on success, the newly opened IO context should be returned here
     * @param url the url to open
     * @param flags a combination of AVIO_FLAG_*
     * @param options a dictionary of additional options, with the same
     *                semantics as in avio_open2()
     * @return 0 on success, a negative AVERROR code on failure
     *
     * @note Certain muxers and demuxers do nesting, i.e. they open one or more
     * additional internal format contexts. Thus the AVFormatContext pointer
     * passed to this callback may be different from the one facing the caller.
     * It will, however, have the same 'opaque' field.
     */
    io_open: (s: AVFormatContext, pb: AVIOContext, url: string, flags: number, options: AVDictionary) => number;
 
    /**
     * A callback for closing the streams opened with AVFormatContext.io_open().
     */
    io_close: (s: AVFormatContext, pb: AVIOContext) => void;
 
    /**
     * ',' separated list of disallowed protocols.
     * - encoding: unused
     * - decoding: set by user
     */
    protocol_blacklist: string;
 
    /**
     * The maximum number of streams.
     * - encoding: unused
     * - decoding: set by user
     */
    max_streams: number;
 
    /**
     * Skip duration calcuation in estimate_timings_from_pts.
     * - encoding: unused
     * - decoding: set by user
     */
    skip_estimate_duration_from_pts: number;
 
    /**
     * Maximum number of packets that can be probed
     * - encoding: unused
     * - decoding: set by user
     */
    max_probe_packets: number;
 
    /**
     * A callback for closing the streams opened with AVFormatContext.io_open().
     *
     * Using this is preferred over io_close, because this can return an error.
     * Therefore this callback is used instead of io_close by the generic
     * libavformat code if io_close is NULL or the default.
     *
     * @param s the format context
     * @param pb IO context to be closed and freed
     * @return 0 on success, a negative AVERROR code on failure
     */
    io_close2: (s: AVFormatContext, pb: AVIOContext) => number;
}
 
/**
 * This function will cause global side data to be injected in the next packet
 * of each stream as well as after any subsequent seek.
 */
export function av_format_inject_global_side_data(s: AVFormatContext) { throw new Error('NotImplemented'); }
 
/**
 * Returns the method used to set ctx->duration.
 *
 * @return AVFMT_DURATION_FROM_PTS, AVFMT_DURATION_FROM_STREAM, or AVFMT_DURATION_FROM_BITRATE.
 */
export function av_fmt_ctx_get_duration_estimation_method(ctx: AVFormatContext): AVDurationEstimationMethod { throw new Error('NotImplemented'); }
 
/**
 * @defgroup lavf_core Core functions
 * @ingroup libavf
 *
 * Functions for querying libavformat capabilities, allocating core structures,
 * etc.
 * @{
 */
 
/**
 * Return the LIBAVFORMAT_VERSION_INT constant.
 */
export function avformat_version(): number { throw new Error("NotImplemented"); }
 
/**
 * Return the libavformat build-time configuration.
 */
export function avformat_configuration(): string { throw new Error("NotImplemented"); }
 
/**
 * Return the libavformat license.
 */
export function avformat_license(): string { throw new Error("NotImplemented"); }
 
/**
 * Do global initialization of network libraries. This is optional,
 * and not recommended anymore.
 *
 * This functions only exists to work around thread-safety issues
 * with older GnuTLS or OpenSSL libraries. If libavformat is linked
 * to newer versions of those libraries, or if you do not use them,
 * calling this function is unnecessary. Otherwise, you need to call
 * this function before any other threads using them are started.
 *
 * This function will be deprecated once support for older GnuTLS and
 * OpenSSL libraries is removed, and this function has no purpose
 * anymore.
 */
export function avformat_network_init(): number { throw new Error("NotImplemented"); }
 
/**
 * Undo the initialization done by avformat_network_init. Call it only
 * once for each time you called avformat_network_init.
 */
export function avformat_network_deinit(): number { throw new Error('NotImplemented'); }
 
/**
 * Iterate over all registered muxers.
 *
 * @param opaque a pointer where libavformat will store the iteration state. Must
 *               point to NULL to start the iteration.
 *
 * @return the next registered muxer or NULL when the iteration is
 *         finished
 */
export function av_muxer_iterate(opaque: OpaquePtr): AVOutputFormat { throw new Error('NotImplemented'); }
 
/**
 * Iterate over all registered demuxers.
 *
 * @param opaque a pointer where libavformat will store the iteration state. Must
 *               point to NULL to start the iteration.
 *
 * @return the next registered demuxer or NULL when the iteration is
 *         finished
 */
export function av_demuxer_iterate(opaque: OpaquePtr): AVInputFormat { throw new Error('NotImplemented'); }
 
/**
 * Allocate an AVFormatContext.
 * avformat_free_context() can be used to free the context and everything
 * allocated by the framework within it.
 */
export function avformat_alloc_context(): AVFormatContext { throw new Error('NotImplemented'); }
 
/**
 * Free an AVFormatContext and all its streams.
 * @param s context to free
 */
export function avformat_free_context(s: AVFormatContext) { throw new Error('NotImplemented'); }
 
/**
 * Get the AVClass for AVFormatContext. It can be used in combination with
 * AV_OPT_SEARCH_FAKE_OBJ for examining options.
 *
 * @see av_opt_find().
 */
export function avformat_get_class(): AVClass { throw new Error('NotImplemented'); }
 
/**
 * Get the AVClass for AVStream. It can be used in combination with
 * AV_OPT_SEARCH_FAKE_OBJ for examining options.
 *
 * @see av_opt_find().
 */
export function av_stream_get_class(): AVClass { throw new Error('NotImplemented'); }
 
/**
 * Add a new stream to a media file.
 *
 * When demuxing, it is called by the demuxer in read_header(). If the
 * flag AVFMTCTX_NOHEADER is set in s.ctx_flags, then it may also
 * be called in read_packet().
 *
 * When muxing, should be called by the user before avformat_write_header().
 *
 * User is required to call avformat_free_context() to clean up the allocation
 * by avformat_new_stream().
 *
 * @param s media file handle
 * @param c unused, does nothing
 *
 * @return newly created stream or NULL on error.
 */
export function avformat_new_stream(s: AVFormatContext, c: AVCodec): AVStream { throw new Error('NotImplemented'); }
 
/**
 * Wrap an existing array as stream side data.
 *
 * @param st stream
 * @param type side information type
 * @param data the side data array. It must be allocated with the av_malloc()
 *             family of functions. The ownership of the data is transferred to
 *             st.
 * @param size side information size
 * @return zero on success, a negative AVERROR code on failure. On failure,
 *         the stream is unchanged and the data remains owned by the caller.
 */
export function av_stream_add_side_data(st: AVStream, type: AVPacketSideDataType,
                            data: Buffer, size: number): number {
    throw new Error("NotImplemented");
}
 
/**
 * Allocate new information from stream.
 *
 * @param stream stream
 * @param type desired side information type
 * @param size side information size
 * @return pointer to fresh allocated data or NULL otherwise
 */
export function av_stream_new_side_data(stream: AVStream, type: AVPacketSideDataType, size: number): Buffer { 
    throw new Error('NotImplemented'); 
}

/**
 * Get side information from stream.
 *
 * @param stream stream
 * @param type desired side information type
 * @param size If supplied, *size will be set to the size of the side data
 *             or to zero if the desired side data is not present.
 * @return pointer to data if present or NULL otherwise
 */
export function av_stream_get_side_data(stream: AVStream, type: AVPacketSideDataType, size: number): Buffer { 
    throw new Error('NotImplemented'); 
}
 
export function av_new_program(s: AVFormatContext, id: number): AVProgram { throw new Error('NotImplemented'); }
 
/**
 * @}
 */
 
 
/**
 * Allocate an AVFormatContext for an output format.
 * avformat_free_context() can be used to free the context and
 * everything allocated by the framework within it.
 *
 * @param *ctx is set to the created format context, or to NULL in
 * case of failure
 * @param oformat format to use for allocating the context, if NULL
 * format_name and filename are used instead
 * @param format_name the name of output format to use for allocating the
 * context, if NULL filename is used instead
 * @param filename the name of the filename to use for allocating the
 * context, may be NULL
 * @return >= 0 in case of success, a negative AVERROR code in case of
 * failure
 */
export function avformat_alloc_output_context2(
    ctx: Out<AVFormatContext>, 
    oformat: AVOutputFormat, 
    format_name: string, 
    filename: string
): number { 
    throw new Error('NotImplemented'); 
}
 
/**
 * @addtogroup lavf_decoding
 * @{
 */
 
/**
 * Find AVInputFormat based on the short name of the input format.
 */
export function av_find_input_format(short_name: string): AVInputFormat { throw new Error('NotImplemented'); }
 
/**
 * Guess the file format.
 *
 * @param pd        data to be probed
 * @param is_opened Whether the file is already opened; determines whether
 *                  demuxers with or without AVFMT_NOFILE are probed.
 */
export function av_probe_input_format(pd: AVProbeData, is_opened: number): AVInputFormat { throw new Error('NotImplemented'); }
 
/**
 * Guess the file format.
 *
 * @param pd        data to be probed
 * @param is_opened Whether the file is already opened; determines whether
 *                  demuxers with or without AVFMT_NOFILE are probed.
 * @param score_max A probe score larger that this is required to accept a
 *                  detection, the variable is set to the actual detection
 *                  score afterwards.
 *                  If the score is <= AVPROBE_SCORE_MAX / 4 it is recommended
 *                  to retry with a larger probe buffer.
 */
export function av_probe_input_format2(pd: AVProbeData, is_opened: number, score_max: Ref<number>): AVInputFormat { 
    throw new Error('NotImplemented'); 
}
 
/**
 * Guess the file format.
 *
 * @param is_opened Whether the file is already opened; determines whether
 *                  demuxers with or without AVFMT_NOFILE are probed.
 * @param score_ret The score of the best detection.
 */
export function av_probe_input_format3(pd: AVProbeData, is_opened: number, score_ret: Out<number>): AVInputFormat { 
    throw new Error('NotImplemented'); 
}
 
/**
 * Probe a bytestream to determine the input format. Each time a probe returns
 * with a score that is too low, the probe buffer size is increased and another
 * attempt is made. When the maximum probe size is reached, the input format
 * with the highest score is returned.
 *
 * @param pb the bytestream to probe
 * @param fmt the input format is put here
 * @param url the url of the stream
 * @param logctx the log context
 * @param offset the offset within the bytestream to probe from
 * @param max_probe_size the maximum probe buffer size (zero for default)
 * @return the score in case of success, a negative value corresponding to an
 *         the maximal score is AVPROBE_SCORE_MAX
 * AVERROR code otherwise
 */
export function av_probe_input_buffer2(
    pb: AVIOContext, 
    fmt: Out<AVInputFormat>,
    url: string, logctx: OpaquePtr,
    offset: number, max_probe_size: number
): number { 
    throw new Error('NotImplemented'); 
}
 
/**
 * Like av_probe_input_buffer2() but returns 0 on success
 */
export function av_probe_input_buffer(
    pb: AVIOContext, fmt: Out<AVInputFormat>,
    url: string, logctx: OpaquePtr,
    offset: number, max_probe_size: number
): number { 
    throw new Error('NotImplemented'); 
}

/**
 * Open an input stream and read the header. The codecs are not opened.
 * The stream must be closed with avformat_close_input().
 *
 * @param ps Pointer to user-supplied AVFormatContext (allocated by avformat_alloc_context).
 *           May be a pointer to NULL, in which case an AVFormatContext is allocated by this
 *           function and written into ps.
 *           Note that a user-supplied AVFormatContext will be freed on failure.
 * @param url URL of the stream to open.
 * @param fmt If non-NULL, this parameter forces a specific input format.
 *            Otherwise the format is autodetected.
 * @param options  A dictionary filled with AVFormatContext and demuxer-private options.
 *                 On return this parameter will be destroyed and replaced with a dict containing
 *                 options that were not found. May be NULL.
 *
 * @return 0 on success, a negative AVERROR on failure.
 *
 * @note If you want to use custom IO, preallocate the format context and set its pb field.
 */
export function avformat_open_input(ps: Ref<AVFormatContext>, url: string,
                        fmt: AVInputFormat, options: Ref<AVDictionary>): number { 
    throw new Error('NotImplemented'); 
}
 
/**
 * Read packets of a media file to get stream information. This
 * is useful for file formats with no headers such as MPEG. This
 * function also computes the real framerate in case of MPEG-2 repeat
 * frame mode.
 * The logical file position is not changed by this function;
 * examined packets may be buffered for later processing.
 *
 * @param ic media file handle
 * @param options  If non-NULL, an ic.nb_streams long array of pointers to
 *                 dictionaries, where i-th member contains options for
 *                 codec corresponding to i-th stream.
 *                 On return each dictionary will be filled with options that were not found.
 * @return >=0 if OK, AVERROR_xxx on error
 *
 * @note this function isn't guaranteed to open all the codecs, so
 *       options being non-empty at return is a perfectly normal behavior.
 *
 * @todo Let the user decide somehow what information is needed so that
 *       we do not waste time getting stuff the user does not need.
 */
export function avformat_find_stream_info(ic: AVFormatContext, options: Ref<AVDictionary>): number { 
    throw new Error('NotImplemented'); 
}
 
/**
 * Find the programs which belong to a given stream.
 *
 * @param ic    media file handle
 * @param last  the last found program, the search will start after this
 *              program, or from the beginning if it is NULL
 * @param s     stream index
 * @return the next program which belongs to s, NULL if no program is found or
 *         the last program is not among the programs of ic.
 */
export function av_find_program_from_stream(ic: AVFormatContext, last: AVProgram, s: number): AVProgram { throw new Error('NotImplemented'); }
 
export function av_program_add_stream_index(ac: AVFormatContext, progid: number, idx: number) { throw new Error('NotImplemented'); }
 
/**
 * Find the "best" stream in the file.
 * The best stream is determined according to various heuristics as the most
 * likely to be what the user expects.
 * If the decoder parameter is non-NULL, av_find_best_stream will find the
 * default decoder for the stream's codec; streams for which no decoder can
 * be found are ignored.
 *
 * @param ic                media file handle
 * @param type              stream type: video, audio, subtitles, etc.
 * @param wanted_stream_nb  user-requested stream number,
 *                          or -1 for automatic selection
 * @param related_stream    try to find a stream related (eg. in the same
 *                          program) to this one, or -1 if none
 * @param decoder_ret       if non-NULL, returns the decoder for the
 *                          selected stream
 * @param flags             flags; none are currently defined
 * @return  the non-negative stream number in case of success,
 *          AVERROR_STREAM_NOT_FOUND if no stream with the requested type
 *          could be found,
 *          AVERROR_DECODER_NOT_FOUND if streams were found but no decoder
 * @note  If av_find_best_stream returns successfully and decoder_ret is not
 *        NULL, then *decoder_ret is guaranteed to be set to a valid AVCodec.
 */
export function av_find_best_stream(ic: AVFormatContext,
                        type: AVMediaType,
                        wanted_stream_nb: number,
                        related_stream: number,
                        decoder_ret: Out<AVCodec>,
                        flags: number): number { 
    throw new Error('NotImplemented'); 
}
 
/**
 * Return the next frame of a stream.
 * This function returns what is stored in the file, and does not validate
 * that what is there are valid frames for the decoder. It will split what is
 * stored in the file into frames and return one for each call. It will not
 * omit invalid data between valid frames so as to give the decoder the maximum
 * information possible for decoding.
 *
 * On success, the returned packet is reference-counted (pkt->buf is set) and
 * valid indefinitely. The packet must be freed with av_packet_unref() when
 * it is no longer needed. For video, the packet contains exactly one frame.
 * For audio, it contains an integer number of frames if each frame has
 * a known fixed size (e.g. PCM or ADPCM data). If the audio frames have
 * a variable size (e.g. MPEG audio), then it contains one frame.
 *
 * pkt->pts, pkt->dts and pkt->duration are always set to correct
 * values in AVStream.time_base units (and guessed if the format cannot
 * provide them). pkt->pts can be AV_NOPTS_VALUE if the video format
 * has B-frames, so it is better to rely on pkt->dts if you do not
 * decompress the payload.
 *
 * @return 0 if OK, < 0 on error or end of file. On error, pkt will be blank
 *         (as if it came from av_packet_alloc()).
 *
 * @note pkt will be initialized, so it may be uninitialized, but it must not
 *       contain data that needs to be freed.
 */
export function av_read_frame(s: AVFormatContext, pkt: AVPacket): number { throw new Error('NotImplemented'); }
 
/**
 * Seek to the keyframe at timestamp.
 * 'timestamp' in 'stream_index'.
 *
 * @param s media file handle
 * @param stream_index If stream_index is (-1), a default
 * stream is selected, and timestamp is automatically converted
 * from AV_TIME_BASE units to the stream specific time_base.
 * @param timestamp Timestamp in AVStream.time_base units
 *        or, if no stream is specified, in AV_TIME_BASE units.
 * @param flags flags which select direction and seeking mode
 * @return >= 0 on success
 */
export function av_seek_frame(s: AVFormatContext, stream_index: number, timestamp: number,
                  flags: number): number { 
    throw new Error('NotImplemented'); 
}
 
/**
 * Seek to timestamp ts.
 * Seeking will be done so that the point from which all active streams
 * can be presented successfully will be closest to ts and within min/max_ts.
 * Active streams are all streams that have AVStream.discard < AVDISCARD_ALL.
 *
 * If flags contain AVSEEK_FLAG_BYTE, then all timestamps are in bytes and
 * are the file position (this may not be supported by all demuxers).
 * If flags contain AVSEEK_FLAG_FRAME, then all timestamps are in frames
 * in the stream with stream_index (this may not be supported by all demuxers).
 * Otherwise all timestamps are in units of the stream selected by stream_index
 * or if stream_index is -1, in AV_TIME_BASE units.
 * If flags contain AVSEEK_FLAG_ANY, then non-keyframes are treated as
 * keyframes (this may not be supported by all demuxers).
 * If flags contain AVSEEK_FLAG_BACKWARD, it is ignored.
 *
 * @param s media file handle
 * @param stream_index index of the stream which is used as time base reference
 * @param min_ts smallest acceptable timestamp
 * @param ts target timestamp
 * @param max_ts largest acceptable timestamp
 * @param flags flags
 * @return >=0 on success, error code otherwise
 *
 * @note This is part of the new seek API which is still under construction.
 */
export function avformat_seek_file(s: AVFormatContext, stream_index: number, min_ts: number, ts: number, max_ts: number, flags: number): number { 
    throw new Error('NotImplemented'); 
}
 
/**
 * Discard all internally buffered data. This can be useful when dealing with
 * discontinuities in the byte stream. Generally works only with formats that
 * can resync. This includes headerless formats like MPEG-TS/TS but should also
 * work with NUT, Ogg and in a limited way AVI for example.
 *
 * The set of streams, the detected duration, stream parameters and codecs do
 * not change when calling this function. If you want a complete reset, it's
 * better to open a new AVFormatContext.
 *
 * This does not flush the AVIOContext (s->pb). If necessary, call
 * avio_flush(s->pb) before calling this function.
 *
 * @param s media file handle
 * @return >=0 on success, error code otherwise
 */
export function avformat_flush(s: AVFormatContext): number { 
    throw new Error('NotImplemented'); 
}
 
/**
 * Start playing a network-based stream (e.g. RTSP stream) at the
 * current position.
 */
export function av_read_play(s: AVFormatContext): number { throw new Error('NotImplemented'); }
 
/**
 * Pause a network-based stream (e.g. RTSP stream).
 *
 * Use av_read_play() to resume it.
 */
export function av_read_pause(s: AVFormatContext): number { throw new Error('NotImplemented'); }
 
/**
 * Close an opened input AVFormatContext. Free it and all its contents
 * and set *s to NULL.
 */
export function avformat_close_input(s: Ref<AVFormatContext>) { 
    throw new Error('NotImplemented'); 
}

/**
 * @}
 */
 
///< seek backward
export const AVSEEK_FLAG_BACKWARD = 1;

///< seeking based on position in bytes
export const AVSEEK_FLAG_BYTE =     2;

///< seek to any frame, even non-keyframes
export const AVSEEK_FLAG_ANY =      4;

///< seeking based on frame number
export const AVSEEK_FLAG_FRAME =    8;

 
/**
 * @addtogroup lavf_encoding
 * @{
 */
 
///< stream parameters initialized in avformat_write_header
export const AVSTREAM_INIT_IN_WRITE_HEADER = 0;

///< stream parameters initialized in avformat_init_output
export const AVSTREAM_INIT_IN_INIT_OUTPUT =  1;

 
/**
 * Allocate the stream private data and write the stream header to
 * an output media file.
 *
 * @param s Media file handle, must be allocated with avformat_alloc_context().
 *          Its oformat field must be set to the desired output format;
 *          Its pb field must be set to an already opened AVIOContext.
 * @param options  An AVDictionary filled with AVFormatContext and muxer-private options.
 *                 On return this parameter will be destroyed and replaced with a dict containing
 *                 options that were not found. May be NULL.
 *
 * @return AVSTREAM_INIT_IN_WRITE_HEADER on success if the codec had not already been fully initialized in avformat_init,
 *         AVSTREAM_INIT_IN_INIT_OUTPUT  on success if the codec had already been fully initialized in avformat_init,
 *         negative AVERROR on failure.
 *
 * @see av_opt_find, av_dict_set, avio_open, av_oformat_next, avformat_init_output.
 * @av_warn_unused_result
 */
export function avformat_write_header(s: AVFormatContext, options: Ref<AVDictionary>): number { 
    throw new Error('NotImplemented'); 
}
 
/**
 * Allocate the stream private data and initialize the codec, but do not write the header.
 * May optionally be used before avformat_write_header to initialize stream parameters
 * before actually writing the header.
 * If using this function, do not pass the same options to avformat_write_header.
 *
 * @param s Media file handle, must be allocated with avformat_alloc_context().
 *          Its oformat field must be set to the desired output format;
 *          Its pb field must be set to an already opened AVIOContext.
 * @param options  An AVDictionary filled with AVFormatContext and muxer-private options.
 *                 On return this parameter will be destroyed and replaced with a dict containing
 *                 options that were not found. May be NULL.
 *
 * @return AVSTREAM_INIT_IN_WRITE_HEADER on success if the codec requires avformat_write_header to fully initialize,
 *         AVSTREAM_INIT_IN_INIT_OUTPUT  on success if the codec has been fully initialized,
 *         negative AVERROR on failure.
 *
 * @see av_opt_find, av_dict_set, avio_open, av_oformat_next, avformat_write_header.
 * @av_warn_unused_result
 */
export function avformat_init_output(s: AVFormatContext, options: Ref<AVDictionary>): number { 
    throw new Error('NotImplemented'); 
}
 
/**
 * Write a packet to an output media file.
 *
 * This function passes the packet directly to the muxer, without any buffering
 * or reordering. The caller is responsible for correctly interleaving the
 * packets if the format requires it. Callers that want libavformat to handle
 * the interleaving should call av_interleaved_write_frame() instead of this
 * function.
 *
 * @param s media file handle
 * @param pkt The packet containing the data to be written. Note that unlike
 *            av_interleaved_write_frame(), this function does not take
 *            ownership of the packet passed to it (though some muxers may make
 *            an internal reference to the input packet).
 *            <br>
 *            This parameter can be NULL (at any time, not just at the end), in
 *            order to immediately flush data buffered within the muxer, for
 *            muxers that buffer up data internally before writing it to the
 *            output.
 *            <br>
 *            Packet's @ref AVPacket.stream_index "stream_index" field must be
 *            set to the index of the corresponding stream in @ref
 *            AVFormatContext.streams "s->streams".
 *            <br>
 *            The timestamps (@ref AVPacket.pts "pts", @ref AVPacket.dts "dts")
 *            must be set to correct values in the stream's timebase (unless the
 *            output format is flagged with the AVFMT_NOTIMESTAMPS flag, then
 *            they can be set to AV_NOPTS_VALUE).
 *            The dts for subsequent packets passed to this function must be strictly
 *            increasing when compared in their respective timebases (unless the
 *            output format is flagged with the AVFMT_TS_NONSTRICT, then they
 *            merely have to be nondecreasing).  @ref AVPacket.duration
 *            "duration") should also be set if known.
 * @return < 0 on error, = 0 if OK, 1 if flushed and there is no more data to flush
 *
 * @see av_interleaved_write_frame()
 */
export function av_write_frame(s: AVFormatContext, pkt: AVPacket): number { throw new Error('NotImplemented'); }
 
/**
 * Write a packet to an output media file ensuring correct interleaving.
 *
 * This function will buffer the packets internally as needed to make sure the
 * packets in the output file are properly interleaved, usually ordered by
 * increasing dts. Callers doing their own interleaving should call
 * av_write_frame() instead of this function.
 *
 * Using this function instead of av_write_frame() can give muxers advance
 * knowledge of future packets, improving e.g. the behaviour of the mp4
 * muxer for VFR content in fragmenting mode.
 *
 * @param s media file handle
 * @param pkt The packet containing the data to be written.
 *            <br>
 *            If the packet is reference-counted, this function will take
 *            ownership of this reference and unreference it later when it sees
 *            fit. If the packet is not reference-counted, libavformat will
 *            make a copy.
 *            The returned packet will be blank (as if returned from
 *            av_packet_alloc()), even on error.
 *            <br>
 *            This parameter can be NULL (at any time, not just at the end), to
 *            flush the interleaving queues.
 *            <br>
 *            Packet's @ref AVPacket.stream_index "stream_index" field must be
 *            set to the index of the corresponding stream in @ref
 *            AVFormatContext.streams "s->streams".
 *            <br>
 *            The timestamps (@ref AVPacket.pts "pts", @ref AVPacket.dts "dts")
 *            must be set to correct values in the stream's timebase (unless the
 *            output format is flagged with the AVFMT_NOTIMESTAMPS flag, then
 *            they can be set to AV_NOPTS_VALUE).
 *            The dts for subsequent packets in one stream must be strictly
 *            increasing (unless the output format is flagged with the
 *            AVFMT_TS_NONSTRICT, then they merely have to be nondecreasing).
 *            @ref AVPacket.duration "duration" should also be set if known.
 *
 * @return 0 on success, a negative AVERROR on error.
 *
 * @see av_write_frame(), AVFormatContext.max_interleave_delta
 */
export function av_interleaved_write_frame(s: AVFormatContext, pkt: AVPacket): number { throw new Error('NotImplemented'); }
 
/**
 * Write an uncoded frame to an output media file.
 *
 * The frame must be correctly interleaved according to the container
 * specification; if not, av_interleaved_write_uncoded_frame() must be used.
 *
 * See av_interleaved_write_uncoded_frame() for details.
 */
export function av_write_uncoded_frame(s: AVFormatContext, stream_index: number,
                           frame: AVFrame): number { 
    throw new Error('NotImplemented'); 
}
 
/**
 * Write an uncoded frame to an output media file.
 *
 * If the muxer supports it, this function makes it possible to write an AVFrame
 * structure directly, without encoding it into a packet.
 * It is mostly useful for devices and similar special muxers that use raw
 * video or PCM data and will not serialize it into a byte stream.
 *
 * To test whether it is possible to use it with a given muxer and stream,
 * use av_write_uncoded_frame_query().
 *
 * The caller gives up ownership of the frame and must not access it
 * afterwards.
 *
 * @return  >=0 for success, a negative code on error
 */
export function av_interleaved_write_uncoded_frame(s: AVFormatContext, stream_index: number,
                                       frame: AVFrame): number { 
    throw new Error('NotImplemented'); 
}
 
/**
 * Test whether a muxer supports uncoded frame.
 *
 * @return  >=0 if an uncoded frame can be written to that muxer and stream,
 *          <0 if not
 */
export function av_write_uncoded_frame_query(s: AVFormatContext, stream_index: number): number { throw new Error('NotImplemented'); }
 
/**
 * Write the stream trailer to an output media file and free the
 * file private data.
 *
 * May only be called after a successful call to avformat_write_header.
 *
 * @param s media file handle
 * @return 0 if OK, AVERROR_xxx on error
 */
export function av_write_trailer(s: AVFormatContext): number { throw new Error('NotImplemented'); }
 
/**
 * Return the output format in the list of registered output formats
 * which best matches the provided parameters, or return NULL if
 * there is no match.
 *
 * @param short_name if non-NULL checks if short_name matches with the
 * names of the registered formats
 * @param filename if non-NULL checks if filename terminates with the
 * extensions of the registered formats
 * @param mime_type if non-NULL checks if mime_type matches with the
 * MIME type of the registered formats
 */
export function av_guess_format(short_name: string, filename: string, mime_type: string): AVOutputFormat { throw new Error('NotImplemented'); }
 
/**
 * Guess the codec ID based upon muxer and filename.
 */
export function av_guess_codec(fmt: AVOutputFormat, short_name: string,
                              filename: string, mime_type: string,
                              type: AVMediaType): AVCodecID { 
    throw new Error('NotImplemented'); 
}
 
/**
 * Get timing information for the data currently output.
 * The exact meaning of "currently output" depends on the format.
 * It is mostly relevant for devices that have an internal buffer and/or
 * work in real time.
 * @param s          media file handle
 * @param stream     stream in the media file
 * @param[out] dts   DTS of the last packet output for the stream, in stream
 *                   time_base units
 * @param[out] wall  absolute time when that packet whas output,
 *                   in microsecond
 * @return  0 if OK, AVERROR(ENOSYS) if the format does not support it
 * Note: some formats or devices may not allow to measure dts and wall
 * atomically.
 */
export function av_get_output_timestamp(s: AVFormatContext, stream: number,
                            dts: Out<number>, wall: Out<number>): number { 
    throw new Error('NotImplemented'); 
}
 
 
/**
 * @}
 */
 
 
/**
 * @defgroup lavf_misc Utility functions
 * @ingroup libavf
 * @{
 *
 * Miscellaneous utility functions related to both muxing and demuxing
 * (or neither).
 */
 
/**
 * Send a nice hexadecimal dump of a buffer to the specified file stream.
 *
 * @param f The file stream pointer where the dump should be sent to.
 * @param buf buffer
 * @param size buffer size
 *
 * @see av_hex_dump_log, av_pkt_dump2, av_pkt_dump_log2
 */
export function av_hex_dump(f: FILE, buf: Buffer, size: number) { throw new Error('NotImplemented'); }
 
/**
 * Send a nice hexadecimal dump of a buffer to the log.
 *
 * @param avcl A pointer to an arbitrary struct of which the first field is a
 * pointer to an AVClass struct.
 * @param level The importance level of the message, lower values signifying
 * higher importance.
 * @param buf buffer
 * @param size buffer size
 *
 * @see av_hex_dump, av_pkt_dump2, av_pkt_dump_log2
 */
export function av_hex_dump_log(avcl: OpaquePtr, level: number, buf: Buffer, size: number) { throw new Error('NotImplemented'); }
 
/**
 * Send a nice dump of a packet to the specified file stream.
 *
 * @param f The file stream pointer where the dump should be sent to.
 * @param pkt packet to dump
 * @param dump_payload True if the payload must be displayed, too.
 * @param st AVStream that the packet belongs to
 */
export function av_pkt_dump2(f: FILE, pkt: AVPacket, dump_payload: number, st: AVStream) { throw new Error('NotImplemented'); }
 
 
/**
 * Send a nice dump of a packet to the log.
 *
 * @param avcl A pointer to an arbitrary struct of which the first field is a
 * pointer to an AVClass struct.
 * @param level The importance level of the message, lower values signifying
 * higher importance.
 * @param pkt packet to dump
 * @param dump_payload True if the payload must be displayed, too.
 * @param st AVStream that the packet belongs to
 */
export function av_pkt_dump_log2(avcl: OpaquePtr, level: number, pkt: AVPacket, dump_payload: number,
                      st: AVStream) { 
    throw new Error('NotImplemented'); 
}
 
/**
 * Get the AVCodecID for the given codec tag tag.
 * If no codec id is found returns AV_CODEC_ID_NONE.
 *
 * @param tags list of supported codec_id-codec_tag pairs, as stored
 * in AVInputFormat.codec_tag and AVOutputFormat.codec_tag
 * @param tag  codec tag to match to a codec ID
 */
export function av_codec_get_id(tags: AVCodecTag[], tag: number): AVCodecID { 
    throw new Error('NotImplemented'); 
}
 
/**
 * Get the codec tag for the given codec id id.
 * If no codec tag is found returns 0.
 *
 * @param tags list of supported codec_id-codec_tag pairs, as stored
 * in AVInputFormat.codec_tag and AVOutputFormat.codec_tag
 * @param id   codec ID to match to a codec tag
 */
export function av_codec_get_tag(tags: AVCodecTag[], id: AVCodecID): number { throw new Error('NotImplemented'); }
 
/**
 * Get the codec tag for the given codec id.
 *
 * @param tags list of supported codec_id - codec_tag pairs, as stored
 * in AVInputFormat.codec_tag and AVOutputFormat.codec_tag
 * @param id codec id that should be searched for in the list
 * @param tag A pointer to the found tag
 * @return 0 if id was not found in tags, > 0 if it was found
 */
export function av_codec_get_tag2(tags: AVCodecTag[], id: AVCodecID,
                      tag: Out<number>): number { throw new Error('NotImplemented'); }
 
export function av_find_default_stream_index(s: AVFormatContext): number { throw new Error('NotImplemented'); }
 
/**
 * Get the index for a specific timestamp.
 *
 * @param st        stream that the timestamp belongs to
 * @param timestamp timestamp to retrieve the index for
 * @param flags if AVSEEK_FLAG_BACKWARD then the returned index will correspond
 *                 to the timestamp which is <= the requested one, if backward
 *                 is 0, then it will be >=
 *              if AVSEEK_FLAG_ANY seek to any frame, only keyframes otherwise
 * @return < 0 if no such timestamp could be found
 */
export function av_index_search_timestamp(st: AVStream, timestamp: number, flags: number): number { throw new Error('NotImplemented'); }
 
/**
 * Get the index entry count for the given AVStream.
 *
 * @param st stream
 * @return the number of index entries in the stream
 */
export function avformat_index_get_entries_count(st: AVStream): number { 
    throw new Error('NotImplemented'); 
}
 
/**
 * Get the AVIndexEntry corresponding to the given index.
 *
 * @param st          Stream containing the requested AVIndexEntry.
 * @param idx         The desired index.
 * @return A pointer to the requested AVIndexEntry if it exists, NULL otherwise.
 *
 * @note The pointer returned by this function is only guaranteed to be valid
 *       until any function that takes the stream or the parent AVFormatContext
 *       as input argument is called.
 */
export function avformat_index_get_entry(st: AVStream, idx: number): AVIndexEntry { 
    throw new Error('NotImplemented'); 
}
 
/**
 * Get the AVIndexEntry corresponding to the given timestamp.
 *
 * @param st          Stream containing the requested AVIndexEntry.
 * @param timestamp   Timestamp to retrieve the index entry for.
 * @param flags       If AVSEEK_FLAG_BACKWARD then the returned entry will correspond
 *                    to the timestamp which is <= the requested one, if backward
 *                    is 0, then it will be >=
 *                    if AVSEEK_FLAG_ANY seek to any frame, only keyframes otherwise.
 * @return A pointer to the requested AVIndexEntry if it exists, NULL otherwise.
 *
 * @note The pointer returned by this function is only guaranteed to be valid
 *       until any function that takes the stream or the parent AVFormatContext
 *       as input argument is called.
 */
export function avformat_index_get_entry_from_timestamp(st: AVStream,
                                                wanted_timestamp: number,
                                                flags: number): AVIndexEntry { 
    throw new Error('NotImplemented'); 
}
/**
 * Add an index entry into a sorted list. Update the entry if the list
 * already contains it.
 *
 * @param timestamp timestamp in the time base of the given stream
 */
export function av_add_index_entry(st: AVStream, pos: number, timestamp: number,
                       size: number, distance: number, flags: number): number { 
    throw new Error('NotImplemented'); 
}
 
 
/**
 * Split a URL string into components.
 *
 * The pointers to buffers for storing individual components may be null,
 * in order to ignore that component. Buffers for components not found are
 * set to empty strings. If the port is not found, it is set to a negative
 * value.
 *
 * @param proto the buffer for the protocol
 * @param proto_size the size of the proto buffer
 * @param authorization the buffer for the authorization
 * @param authorization_size the size of the authorization buffer
 * @param hostname the buffer for the host name
 * @param hostname_size the size of the hostname buffer
 * @param port_ptr a pointer to store the port number in
 * @param path the buffer for the path
 * @param path_size the size of the path buffer
 * @param url the URL to split
 * @nodeTODO 
 */
export function av_url_split(
    proto: string,
    proto_size: number,
    authorization: string,
    authorization_size: number,
    hostname: string,
    hostname_size: number,
    port_ptr: Out<number>,
    path: string,
    path_size: number,
    url: string
) { 
    throw new Error('NotImplemented'); 
}
 
 
/**
 * Print detailed information about the input or output format, such as
 * duration, bitrate, streams, container, programs, metadata, side data,
 * codec and time base.
 *
 * @param ic        the context to analyze
 * @param index     index of the stream to dump information about
 * @param url       the URL to print, such as source or destination file
 * @param is_output Select whether the specified context is an input(0) or output(1)
 */
export function av_dump_format(ic: AVFormatContext,
                    index: number,
                    url: string,
                    is_output: number) { throw new Error('NotImplemented'); }

///< Allow multiple %d
export const AV_FRAME_FILENAME_FLAGS_MULTIPLE = 1;
 
/**
 * Return in 'buf' the path with '%d' replaced by a number.
 *
 * Also handles the '%0nd' format where 'n' is the total number
 * of digits and '%%'.
 *
 * @param buf destination buffer
 * @param buf_size destination buffer size
 * @param path numbered sequence string
 * @param number frame number
 * @param flags AV_FRAME_FILENAME_FLAGS_*
 * @return 0 if OK, -1 on format error
 */
export function av_get_frame_filename2(
    buf: string, buf_size: number,
    path: string, number: number, 
    flags: number
): number { throw new Error('NotImplemented'); };
 
export function av_get_frame_filename(
    buf: string, buf_size: number,
    path: string, number: number
): number { throw new Error('NotImplemented'); };
 
/**
 * Check whether filename actually is a numbered sequence generator.
 *
 * @param filename possible numbered sequence string
 * @return 1 if a valid numbered sequence string, 0 otherwise
 */
export function av_filename_number_test(filename: string): number { throw new Error('NotImplemented'); }
 
/**
 * Generate an SDP for an RTP session.
 *
 * Note, this overwrites the id values of AVStreams in the muxer contexts
 * for getting unique dynamic payload types.
 *
 * @param ac array of AVFormatContexts describing the RTP streams. If the
 *           array is composed by only one context, such context can contain
 *           multiple AVStreams (one AVStream per RTP stream). Otherwise,
 *           all the contexts in the array (an AVCodecContext per RTP stream)
 *           must contain only one AVStream.
 * @param n_files number of AVCodecContexts contained in ac
 * @param buf buffer where the SDP will be stored (must be allocated by
 *            the caller)
 * @param size the size of the buffer
 * @return 0 if OK, AVERROR_xxx on error
 */
export function av_sdp_create(ac: AVFormatContext[], n_files: number, buf: Buffer, size: number): number { throw new Error('NotImplemented'); }
 
/**
 * Return a positive value if the given filename has one of the given
 * extensions, 0 otherwise.
 *
 * @param filename   file name to check against the given extensions
 * @param extensions a comma-separated list of filename extensions
 */
export function av_match_ext(filename: string, extensions: string): number { throw new Error('NotImplemented'); }
 
/**
 * Test if the given container can store a codec.
 *
 * @param ofmt           container to check for compatibility
 * @param codec_id       codec to potentially store in container
 * @param std_compliance standards compliance level, one of FF_COMPLIANCE_*
 *
 * @return 1 if codec with ID codec_id can be stored in ofmt, 0 if it cannot.
 *         A negative number if this information is not available.
 */
export function avformat_query_codec(ofmt: AVOutputFormat, codec_id: AVCodecID,
                         std_compliance: number): number { 
    throw new Error('NotImplemented'); 
}
 
/**
 * @defgroup riff_fourcc RIFF FourCCs
 * @{
 * Get the tables mapping RIFF FourCCs to libavcodec AVCodecIDs. The tables are
 * meant to be passed to av_codec_get_id()/av_codec_get_tag() as in the
 * following code:
 * @code
 * uint32_t tag = MKTAG('H', '2', '6', '4');
 * const struct AVCodecTag *table[] = { avformat_get_riff_video_tags(), 0 };
 * enum AVCodecID id = av_codec_get_id(table, tag);
 * @endcode
 */
/**
 * @return the table mapping RIFF FourCCs for video to libavcodec AVCodecID.
 */
export function avformat_get_riff_video_tags(): AVCodecTag[] { 
    throw new Error('NotImplemented'); 
}

/**
 * @return the table mapping RIFF FourCCs for audio to AVCodecID.
 */
export function avformat_get_riff_audio_tags(): AVCodecTag[] { 
    throw new Error('NotImplemented'); 
}

/**
 * @return the table mapping MOV FourCCs for video to libavcodec AVCodecID.
 */
export function avformat_get_mov_video_tags(): AVCodecTag[] { 
    throw new Error('NotImplemented'); 
}

/**
 * @return the table mapping MOV FourCCs for audio to AVCodecID.
 */
export function avformat_get_mov_audio_tags(): AVCodecTag[] { 
    throw new Error('NotImplemented'); 
}
 
/**
 * @}
 */
 
/**
 * Guess the sample aspect ratio of a frame, based on both the stream and the
 * frame aspect ratio.
 *
 * Since the frame aspect ratio is set by the codec but the stream aspect ratio
 * is set by the demuxer, these two may not be equal. This function tries to
 * return the value that you should use if you would like to display the frame.
 *
 * Basic logic is to use the stream aspect ratio if it is set to something sane
 * otherwise use the frame aspect ratio. This way a container setting, which is
 * usually easy to modify can override the coded value in the frames.
 *
 * @param format the format context which the stream is part of
 * @param stream the stream which the frame is part of
 * @param frame the frame with the aspect ratio to be determined
 * @return the guessed (valid) sample_aspect_ratio, 0/1 if no idea
 */
export function av_guess_sample_aspect_ratio(format: AVFormatContext, stream: AVStream, frame: AVFrame): AVRational { throw new Error('NotImplemented'); }
 
/**
 * Guess the frame rate, based on both the container and codec information.
 *
 * @param ctx the format context which the stream is part of
 * @param stream the stream which the frame is part of
 * @param frame the frame for which the frame rate should be determined, may be NULL
 * @return the guessed (valid) frame rate, 0/1 if no idea
 */
export function av_guess_frame_rate(ctx: AVFormatContext, stream: AVStream, frame: AVFrame): AVRational { throw new Error('NotImplemented'); }
 
/**
 * Check if the stream st contained in s is matched by the stream specifier
 * spec.
 *
 * See the "stream specifiers" chapter in the documentation for the syntax
 * of spec.
 *
 * @return  >0 if st is matched by spec;
 *          0  if st is not matched by spec;
 *          AVERROR code if spec is invalid
 *
 * @note  A stream specifier can match several streams in the format.
 */
export function avformat_match_stream_specifier(s: AVFormatContext, st: AVStream,
                                    spec: string): number { 
    throw new Error('NotImplemented'); 
}
 
export function avformat_queue_attached_pictures(s: AVFormatContext): number { 
    throw new Error('NotImplemented'); 
}
 
enum AVTimebaseSource {
    AVFMT_TBCF_AUTO = -1,
    AVFMT_TBCF_DECODER,
    AVFMT_TBCF_DEMUXER,
    /**
     * @if FF_API_R_FRAME_RATE
     */
    AVFMT_TBCF_R_FRAMERATE
};
 
/**
 * Transfer internal timing information from one stream to another.
 *
 * This function is useful when doing stream copy.
 *
 * @param ofmt     target output format for ost
 * @param ost      output stream which needs timings copy and adjustments
 * @param ist      reference input stream to copy timings from
 * @param copy_tb  define from where the stream codec timebase needs to be imported
 */
export function avformat_transfer_internal_stream_timing_info(ofmt: AVOutputFormat,
                                                  ost: AVStream, ist: AVStream,
                                                  copy_tb: AVTimebaseSource): number { 
    throw new Error('NotImplemented'); 
}
 
/**
 * Get the internal codec timebase from a stream.
 *
 * @param st  input stream to extract the timebase from
 */
export function av_stream_get_codec_timebase(st: AVStream): AVRational { throw new Error('NotImplemented'); }
