import { describe } from "razmin";
import { AVDictionary as AVDictionaryType, AV_DICT_DONT_OVERWRITE, AV_DICT_MULTIKEY } from "./dict";
import { AVDictionary as AVDictionaryImpl } from "../../binding";
import { expect } from "chai";

const AVDictionary = <typeof AVDictionaryType>AVDictionaryImpl;
describe("AVDictionary", it => {
    it("can be constructed", () => {
        new AVDictionary();
    });

    it("can set a key", () => {
        new AVDictionary().set("foo", "bar");
    });
    it("can set and get a key", () => {
        let dict = new AVDictionary()
        dict.set("foo", "bar");
        expect(dict.get("foo").value).to.equal("bar");
    });
    it("returns undefined for a missing key", () => {
        let dict = new AVDictionary()
        expect(dict.get("foo")).to.equal(undefined);
    });
    it("can clear a key", () => {
        let dict = new AVDictionary()
        dict.set("foo", "bar");
        expect(dict.keys).to.eql(["foo"]);
        dict.set("foo", null);
        expect(dict.keys).to.eql([]);
    });
    it("can set a key multiple times with the right flags", () => {
        let dict = new AVDictionary()
        dict.set("foo", "bar", AV_DICT_MULTIKEY);
        dict.set("foo", "bar2", AV_DICT_MULTIKEY);
        expect(dict.keys).to.eql(["foo", "foo"]);
    });
    it("can read back multiple entries with the same key", () => {
        let dict = new AVDictionary()
        dict.set("foo", "bar", AV_DICT_MULTIKEY);
        dict.set("foo", "bar2", AV_DICT_MULTIKEY);

        let entry = dict.get("foo");
        let entry2 = dict.get("foo", entry);

        expect(entry.value).to.equal("bar");
        expect(entry2.value).to.equal("bar2");
    });
    it("can clear all keys", () => {
        let dict = new AVDictionary()
        dict.set("foo", "bar");
        dict.set("foo2", "bar");
        expect(dict.keys).to.eql(["foo", "foo2"]);

        dict.clear();
        expect(dict.keys).to.eql([]);
    });
});