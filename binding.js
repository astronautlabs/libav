try {
    module.exports = require(`./build/Debug/libav.node`);
} catch (error) {
    module.exports = require(`./build/Release/libav.node`);
}