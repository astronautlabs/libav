import "zone.js";
import { suite } from "razmin";

console.log(`##### PID ${process.pid} #####`);

suite()
    .withOptions({ 
        execution: { 
            timeout: 20_000 
        }
    })
    .include(["**/*.test.js"])
    .run()
;