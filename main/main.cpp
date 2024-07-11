#include <cstdio>
#include <driver/spi_master.h>
#include <array>

#include "inky_impression7_3.h"
#include "images.h"


extern "C" [[maybe_unused]] [[noreturn]]
void app_main() {
    InkyImpression7_3f::init_hw();
    puts("end write");
    InkyImpression7_3f::set_border(InkyImpression7_3f::BLUE);
    InkyImpression7_3f::set_background(image1.data());
    InkyImpression7_3f::set_border(InkyImpression7_3f::BLUE);
    for (int i = 100; i <400; ++i) {
        InkyImpression7_3f::set_pixel(200 + i, 50 + i, InkyImpression7_3f::WHITE);
        InkyImpression7_3f::set_pixel(201 + i, 50 + i, InkyImpression7_3f::YELLOW);
        InkyImpression7_3f::set_pixel(202 + i, 50 + i, InkyImpression7_3f::ORANGE);
        InkyImpression7_3f::set_pixel(203 + i, 50 + i, InkyImpression7_3f::RED);
        InkyImpression7_3f::set_pixel(204 + i, 50 + i, InkyImpression7_3f::GREEN);
        InkyImpression7_3f::set_pixel(205 + i, 50 + i, InkyImpression7_3f::BLUE);
        InkyImpression7_3f::set_pixel(206 + i, 50 + i, InkyImpression7_3f::BLACK);
    }
    InkyImpression7_3f::show();
    while (true) {
        puts("End_loop");
        vTaskDelay(50);
    }

}
