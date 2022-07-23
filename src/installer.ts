import fetchImpl from 'node-fetch';
import os from 'os';
import fs from 'fs';
import path from 'path';
import * as unzip from 'unzipper';
import rimraf from 'rimraf';
import * as stream from 'stream';

globalThis.fetch = fetchImpl;

async function getCatalog(): Promise<Record<string,string>> {
    let response = await fetch("https://github.com/BtbN/FFmpeg-Builds/wiki/Latest");
    if (response.status >= 400)
        throw new Error(`Failed to fetch ffmpeg build catalog: status ${response.status}, body: '${await response.text()}'`)

    let text = await response.text();
    let matches = Array.from(text.matchAll(/(https:\/\/github.com\/BtbN\/FFmpeg-Builds\/releases\/download[^"]+)">([^<]+)<\/a>/g))

    return matches.reduce((o, [_, url, type]) => (o[type] = url, o), <Record<string,string>>{});
}

const PLATFORM_ARCH_MAPPINGS = {
    'win32|x64': 'win64',
    'linux|x64': 'linux64',
    'linux|arm64': 'linuxarm64'
}

function getBuildName() {
    let buildName: string = PLATFORM_ARCH_MAPPINGS[`${os.platform()}|${os.arch()}`];

    return buildName;
}

/**
 * Install FFmpeg from BtbN's FFmpeg-Builds service.
 */
async function main() {
    let buildName = getBuildName();
    let catalog = await getCatalog();
    let gpl = false;
    let buildFile = catalog[`${buildName}-${gpl ? 'gpl' : 'lgpl'}-shared-5.0`];
    if (!buildName && os.platform() === 'win32')
        throw new Error(`No build available for platform=${os.platform()}, arch=${os.arch()}`);

    let buildFileName = path.basename(buildFile);
    let buildFileResponse = await fetch(buildFile);
    let body = <fs.ReadStream><unknown>buildFileResponse.body;
    let distDir = path.join(__dirname);

    await removeDir(path.join(distDir, 'ffmpeg'));
    await finishStream(body.pipe(unzip.Extract({ path: distDir })));
    await rename(
        path.join(distDir, path.basename(buildFileName, '.zip')), 
        path.join(distDir, `ffmpeg`)
    );
}

async function removeDir(path: string) {
    //console.log(`removeDir '${path}'`);
    await new Promise<void>((rs, rj) => rimraf(path, {}, (e) => e ? rj(e) : rs()));
}

async function rename(from: string, to: string) {
    //console.log(`rename '${from}' -> '${to}'`);
    await new Promise<void>((rs, rj) => fs.rename(from, to, (err) => err ? rj(err) : rs()));
}

async function finishStream(stream: stream.Writable) {
    await new Promise<void>((rs, rj) => {
        stream.on('end', () => rs());
        stream.on('finish', () => rs());
        stream.on('error', e => rj(e));
    });
}

main();