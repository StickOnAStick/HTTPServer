#pragma once

/*
Core 0 is responsible for accepting incoming requests and placing them on the queue.

Core 1 is responsible for processing the requests place onto said queue.
*/

void core0_main();