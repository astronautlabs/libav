import { expect } from "chai";
import { delay, describe } from "razmin";
import { AVCodec as AVCodecImpl } from "../../binding";
import { AVCodec as AVCodecType } from "./codec";
import { AVCodecID } from "./codec_id";

const AVCodec = <typeof AVCodecType>AVCodecImpl;

describe('AVCodec', it => {
    it("should enumerate many codecs", () => {
        expect(AVCodec.all().length).to.be.greaterThan(100);
    });
    it("should be able to find a decoder by ID", async () => {
        await delay(1000);
        expect(typeof AVCodecID.AV_CODEC_ID_APNG).to.equal('number');
        expect(AVCodec.findDecoder(AVCodecID.AV_CODEC_ID_APNG)).to.exist;
        expect(AVCodec.findDecoder(AVCodecID.AV_CODEC_ID_APNG).id).to.equal(AVCodecID.AV_CODEC_ID_APNG);
    });
    it("should be able to find a decoder by name", async () => {
        await delay(1000);
        expect(AVCodec.findDecoder("apng")).to.exist;
        expect(AVCodec.findDecoder("apng").id).to.equal(AVCodecID.AV_CODEC_ID_APNG);
    });
});