#include <cstdio>
#include <driver/spi_master.h>
#include <array>

#include "inky_impression7_3.h"
#include "images.h"

InkyImpression7_3f inky;
extern "C" [[maybe_unused]] [[noreturn]]
void app_main() {
    inky.init_hw();
    inky.set_border(InkyImpression7_3f::BLUE);
    inky.set_background(image1.data());
//    for (int i = 100; i <400; ++i) {
//        inky.set_pixel(200 + i, 50 + i, InkyImpression7_3f::WHITE);
//        inky.set_pixel(201 + i, 50 + i, InkyImpression7_3f::YELLOW);
//        inky.set_pixel(202 + i, 50 + i, InkyImpression7_3f::ORANGE);
//        inky.set_pixel(203 + i, 50 + i, InkyImpression7_3f::RED);
//        inky.set_pixel(204 + i, 50 + i, InkyImpression7_3f::GREEN);
//        inky.set_pixel(205 + i, 50 + i, InkyImpression7_3f::BLUE);
//        inky.set_pixel(206 + i, 50 + i, InkyImpression7_3f::BLACK);
//    }
    inky.show();
    while (true) {
        puts("End_loop");
        vTaskDelay(50);
    }

}
