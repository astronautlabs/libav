try {
    module.exports = require(`./build/Debug/libav.node`);
} catch (error) {
    console.error(`couldnt load debug: ${error.message}`);
    try {
        module.exports = require(`./build/Release/libav.node`);
    } catch (error2) {
        console.error(`Failed to load debug version:`);
        console.error(error);
        console.log();
        console.error(`Failed to load release version:`);
        console.error(error2);
        console.log();

        throw error2;
    }
}