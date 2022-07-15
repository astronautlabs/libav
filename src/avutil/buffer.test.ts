import { describe } from "razmin";
import { AVBuffer as AVBufferImpl, AVBufferPool as AVBufferPoolImpl } from "../../binding";
import type { AVBuffer as AVBufferType, AVBufferPool as AVBufferPoolType } from "./buffer";
import { expect } from 'chai';

const AVBuffer = <typeof AVBufferType>AVBufferImpl;
const AVBufferPool = <typeof AVBufferPoolType>AVBufferPoolImpl;

describe("AVBuffer", it => {
    it('can be constructed with a size', () => {
        let buf = new AVBuffer(123);
        expect(buf.size).to.equal(123);
        expect(buf.data.byteLength).to.equal(123);
    });

    it('zeroes memory when requested', () => {
        let buf = new AVBuffer(4096, true);
        let typedArray = new Uint8Array(buf.data);
        for (let value of typedArray)
            expect(value).to.equal(0);
    });

    it('can be constructed with an ArrayBuffer', () => {
        let arrayBuffer = new ArrayBuffer(123);
        let buf = new AVBuffer(arrayBuffer);
        expect(buf.size).to.equal(123);
    });

    it('shares data with source ArrayBuffer', () => {
        let arrayBuffer = new ArrayBuffer(123);
        let typedArray1 = new Uint8Array(arrayBuffer);

        typedArray1[0] = 23;

        let buf = new AVBuffer(arrayBuffer);
        let typedArray2 = new Uint8Array(buf.data);

        expect(typedArray2[0]).to.equal(23);

        typedArray1[0] = 45;
        expect(typedArray2[0]).to.equal(45);
    });

    it('shows as writeable when referenced only once', () => {
        let buf = new AVBuffer(123);
        expect(buf.writable).to.be.true;
    });
    it('can be reallocated', () => {
        let buf = new AVBuffer(123);

        expect(buf.size).to.equal(123);
        expect(buf.data.byteLength).to.equal(123);

        let typedArray1 = new Uint8Array(buf.data);

        typedArray1[0] = 44;

        buf.realloc(124);
        
        expect(buf.size).to.equal(124);
        expect(buf.data.byteLength).to.equal(124);

        let typedArray2 = new Uint8Array(buf.data);
        expect(typedArray2[0]).to.equal(44);
    });
})

describe("AVBufferPool", it => {
    it("can be constructed with a size", () => {
        new AVBufferPool(123);
    })

    it("can issue a buffer", () => {
        let pool = new AVBufferPool(123);
        let buffer = pool.get();

        expect(buffer instanceof AVBuffer).to.be.true;
        expect(buffer.size).to.equal(123);
    })

    it("can reuse a buffer if its no longer used", () => {
        let pool = new AVBufferPool(123);
        let buffer = pool.get();
        expect(buffer.data).to.exist;
        new Uint8Array(buffer.data)[0] = 37;
        buffer.free();
        buffer = null;
        gc();

        buffer = pool.get();
        expect(buffer.data).to.exist;
        expect(new Uint8Array(buffer.data)[0]).to.equal(37);
    })

    it("will not reuse a buffer if its still in use", () => {
        let pool = new AVBufferPool(123);
        let buffer = pool.get();
        expect(buffer.data).to.exist;
        new Uint8Array(buffer.data)[0] = 37;

        buffer = pool.get();
        expect(buffer.data).to.exist;
        expect(new Uint8Array(buffer.data)[0]).not.to.equal(37);
    })
});