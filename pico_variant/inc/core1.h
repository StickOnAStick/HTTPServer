#pragma once

/* Core 1 is responsible for processing requests placed onto the queue. 

    Core 0 is responsible for place requests onto said buffer.

*/

void core1_main();