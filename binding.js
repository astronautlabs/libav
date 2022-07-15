try {
    module.exports = require(`./build/Debug/libav.node`);
} catch (error) {
    console.error(`couldnt load debug: ${error.message}`);
    module.exports = require(`./build/Release/libav.node`);
}