import { expect } from "chai";
import { describe } from "razmin";
import { AVMediaType, AVPictureType, AVUtil } from "..";

describe("AVUtil", it => {
    it("provides version", () => expect(AVUtil.version).to.be.a.string);
    it("provides versionInfo", () => expect(AVUtil.versionInfo).to.be.a.string);
    it("provides configuration", () => expect(AVUtil.configuration).to.be.a.string);
    it("provides license", () => expect(AVUtil.license).to.be.a.string);
    it("provides timebase", () => expect(AVUtil.timebase).to.eql({ num: 1, den: 1000000 }));
    it("can decode FourCC codes", () => expect(AVUtil.getFourCCString(0x31637661)).to.equal('avc1'));
    it("can provide AVMediaType strings", () => expect(AVUtil.getMediaTypeString(AVMediaType.AVMEDIA_TYPE_AUDIO)).to.equal('audio'));
    it("can provide AVPictureType strings", () => expect(AVUtil.getPictureTypeString(AVPictureType.AV_PICTURE_TYPE_SI)).to.equal('i'));
});