#include <cstdio>
#include <driver/spi_master.h>
#include <array>

#include "inky_impression7_3.h"
#include "images.h"


void inky_background(const uint8_t *packet_pixels) ;

extern "C" [[maybe_unused]] [[noreturn]]
void app_main() {
    inky_init_hw();
////    for (int y = 0; y < DISP_HEIGHT; ++y) {
////        const char *lineStr = test1_xpm[y + 8];
////        for (int x = 0; x < DISP_WIDTH; ++x) {
////            COLOR c;
////            switch (lineStr[x]) {
////                case ' ':
////                    c = BLACK;
////                    break;
////                case '.':
////                    c = WHITE;
////                    break;
////                case '+':
////                    c = GREEN;
////                    break;
////                case '@':
////                    c = BLUE;
////                    break;
////                case '#':
////                    c = RED;
////                    break;
////                case '$':
////                    c = YELLOW;
////                    break;
////                case '%':
////                    c = ORANGE;
////                    break;
////            }
////            inky_set_pixel(x, y, c);
////        }
//    }

    puts("end write");
    inky_set_border(BLUE);
    inky_background(image1.data());
    inky_set_border(COLOR::BLUE);
    for (int i = 100; i <400; ++i) {
        inky_set_pixel(200 + i, 50 + i, COLOR::WHITE);
        inky_set_pixel(201 + i, 50 + i, COLOR::YELLOW);
        inky_set_pixel(202 + i, 50 + i, COLOR::ORANGE);
        inky_set_pixel(203 + i, 50 + i, COLOR::RED);
        inky_set_pixel(204 + i, 50 + i, COLOR::GREEN);
        inky_set_pixel(205 + i, 50 + i, COLOR::BLUE);
        inky_set_pixel(206 + i, 50 + i, COLOR::BLACK);
    }
    inky_show();
    while (true) {
        puts("End_loop");
        vTaskDelay(50);
    }

}
