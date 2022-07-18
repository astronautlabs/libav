# @/libav

Implements native libav* bindings for Node.js.

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

