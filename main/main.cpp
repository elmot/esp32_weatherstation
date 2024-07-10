#include <cstdio>
#include <driver/spi_master.h>

#include "inky_impression7_3.h"

void inky_set_border(COLOR);

extern "C" [[maybe_unused]] [[noreturn]]
void app_main() {

    inky_init_hw();
    for (int y = 0; y < DISP_HEIGHT; ++y) {
        for (int x = 0; x < DISP_WIDTH; ++x) {
            inky_set_pixel(x, y, (COLOR) ((x+y) % 8));
        }
    }
    inky_show();
    for (int y = 0; y < DISP_HEIGHT; ++y) {
        for (int x = 0; x < DISP_WIDTH; ++x) {
            inky_set_pixel(x, y, (COLOR) ((3 + y / 100 + x / 110) % 8));
        }
    }
    puts("end write");
    inky_set_border(BLUE);
    inky_show();
    while (true) {
        puts("End_loop");
        vTaskDelay(50);
    }

}

