import { delay, describe } from "razmin";
import { AVCodecContext as AVCodecContextType, AVCodecContextError } from "./avcodec";
import { AVCodecContext as AVCodecContextImpl } from "../../binding";
import { AVCodec as AVCodecType } from "./codec";
import { AVCodec as AVCodecImpl } from "../../binding";
import { AVFrame as AVFrameType, AVPixelFormat } from "../avutil";
import { AVFrame as AVFrameImpl } from "../../binding";
import { AVPacket as AVPacketType } from "./packet";
import { AVPacket as AVPacketImpl } from "../../binding";
import { expect } from "chai";

const AVCodecContext = <typeof AVCodecContextType>AVCodecContextImpl;
const AVCodec = <typeof AVCodecType>AVCodecImpl;
const AVFrame = <typeof AVFrameType>AVFrameImpl;
const AVPacket = <typeof AVPacketType>AVPacketImpl;

describe("AVCodecContext", it => {
    function createEncoderContext(encoder: string) {
        let codec = AVCodec.findEncoder('rawvideo');
        let context = new AVCodecContext(codec);
        context.bitRate = 400000;
        context.width = 352;
        context.height = 288;
        context.timeBase = { num: 1, den: 25 };
        context.frameRate = { num: 25, den: 1 };
        context.gopSize = 10;
        context.maxBFrames = 1;
        context.pixelFormat = AVPixelFormat.AV_PIX_FMT_YUV420P;

        return context;
    }

    function createTestFrame(context: AVCodecContextType) {
        let frame = new AVFrame();
        frame.format = context.pixelFormat;
        frame.width = context.width;
        frame.height = context.height;
        frame.allocateBuffer();
        frame.pts = 0;

        return frame;
    }

    function createInvalidFrame(context: AVCodecContextType) {
        return new AVFrame();
    }

    it('should be able to perform a simple encoding', async () => {
        await delay(250);

        let context = createEncoderContext('rawvideo');
        let frame = createTestFrame(context);

        let resolve, reject;
        let promise = new Promise<void>((rs, rj) => (resolve = rs, reject = rj));
        let count = 0;

        context.onPacket = () => (count += 1, resolve());
        context.open();
        context.sendFrame(frame);

        await promise;
        await delay(250);

        expect(count).to.equal(1);
    });

    it('should encode one packet for one frame (when using a trivial encoder)', async () => {
        await delay(250);

        let context = createEncoderContext('rawvideo');
        let count = 0;

        context.onPacket = () => count += 1;
        context.open();
        context.sendFrame(createTestFrame(context));
        await delay(250);
        expect(count).to.equal(1);
        context.sendFrame(createTestFrame(context));
        await delay(250);
        expect(count).to.equal(2);
        context.sendFrame(createTestFrame(context));
        await delay(250);
        expect(count).to.equal(3);
    });

    it('should wait for onPacket to be registered before sending packets', async () => {
        await delay(250);

        let context = createEncoderContext('rawvideo');
        let count = 0;

        context.open();
        context.sendFrame(createTestFrame(context));
        await delay(250);
        expect(count).to.equal(0);
        context.sendFrame(createTestFrame(context));
        await delay(250);
        expect(count).to.equal(0);
        context.sendFrame(createTestFrame(context));
        await delay(250);
        expect(count).to.equal(0);
        context.onPacket = () => count += 1;
        await delay(250);
        expect(count).to.equal(3);
    });
    it('should convey errors via onError', async () => {
        await delay(250);

        let context = createEncoderContext('rawvideo');

        context.open();

        let caughtErrors: AVCodecContextError[] = [];
        context.onError = (err) => caughtErrors.push(err);
        context.sendFrame(createInvalidFrame(context));
        await delay(250);

        expect(caughtErrors.length).to.equal(1);

        let error = caughtErrors[0];

        expect(error.code).to.equal(`averror:-22`);
        expect(error.message.includes("during avcodec_send")).to.be.true;
    });
});