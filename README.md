# @/libav

> 📝 **Pre-Alpha Quality**  
> This library is incomplete, broken, and in heavy development.

> 📺 Part of the [**Astronaut Labs Broadcast Suite**](https://github.com/astronautlabs/broadcast)

---

Node.js bindings for ffmpeg's `libav*` libraries. The original C APIs are highly adapted for comfortable and efficient use in Typescript and other Javascript flavors.

# Installation

On Linux and macOS you need to install FFmpeg 5.0 or 5.1 (including its shared libraries) yourself. You will also 
need the headers (-dev packages) for the libav* libraries. Most distributions are shipping FFmpeg 4.x, that version is 
not supported by this binding. On Windows a build will be downloaded and used as part of the package installation.

```
npm install libav
```

## Ubuntu

```
sudo apt install ffmpeg libavutil-dev libavformat-dev libavcodec-dev libavdevice-dev libavfilter-dev libswresample-dev libswscale-dev
```
> NOTE: Make sure you have ffmpeg 5.0 or 5.1. The 4.x versions are not supported.

## Enabling GPL features

On Windows by default, the package installer will install an LGPL-compatible version of FFmpeg. If your application's 
license is GPL-compatible, you can enable GPL features on that build by setting the environment variable FFMPEG_ENABLE_GPL=1
before installing the package. 

On other platforms (Linux, macOS), the ffmpeg installation is handled externally, and it is up to you to ensure that you 
properly adhere to the terms of the LGPL or GPL as appropriate. For more information about the important issues 
regarding ffmpeg licensing, please visit https://ffmpeg.org/legal.html.

# Progress

Release Status:
- Pre-alpha quality: Nothing is expected to work, and you shouldn't use this (yet)
- No packages released yet.
- No handling of ffmpeg build yet during install, and the paths are hardcoded.
- Seriously you shouldn't use this yet. 

Libraries:
- `libavutil` -- The common data structures needed to interface with the rest of the library (AVBuffer, AVFrame, AVDictionary, AVRational, etc) are mostly complete.
- `libavcodec` -- The core API and DX is complete.
- `libavformat` -- Only declarations are present, barely supported at all yet.
- `avdevice` -- Not started.
- `avfilter` -- Not started.
- `swresample` -- Not started.
- `swscale` -- Not started.

# Examples

## Packet Encoder

```typescript
    // Find the codec you are interested in (see also AVCodec.all())
    let codec = AVCodec.findEncoder('rawvideo');

    // Construct a new AVCodecContext for it
    let context = codec.newContext();

    // Configure the necessary context parameters
    context.bitRate = 400000;
    context.width = 352;
    context.height = 288;
    context.timeBase = { num: 1, den: 25 };
    context.frameRate = { num: 25, den: 1 };
    context.gopSize = 10;
    context.maxBFrames = 1;
    context.pixelFormat = AVPixelFormat.AV_PIX_FMT_YUV420P;

    // Register a callback to receive packets from the encoder as they arrive
    // Each AVCodecContext maintains a worker thread for performing encoding/decoding.

    context.onPacket = (packet: AVPacket) => {
        // Do something with the newly encoded packet!
    };

    // Open the context, this will finish preparing it to be used,
    // based on the configuration above

    context.open();

    // Construct or acquire a frame to encode

    let frame = new AVFrame();
    frame.format = context.pixelFormat;
    frame.width = context.width;
    frame.height = context.height;
    frame.allocateBuffer();
    frame.pts = 0;

    // Send the frame to the encoder
    // The resulting packets will arrive via onPacket

    context.sendFrame(frame);
```

