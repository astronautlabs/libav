
export interface OpaquePtr { }

export interface Out<T> {
    value: T;
}

export interface Ref<T> {
    value: T;
}

export type FILE = OpaquePtr;

export class NotImplemented extends Error {
    constructor() {
        super("Not implemented");
    }
}