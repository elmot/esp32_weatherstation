//
// Created by elmot on 10 Jul 2024.
//

#ifndef ESP32_WEATHERSTATION_INKY_IMPRESSION7_3_H
#define ESP32_WEATHERSTATION_INKY_IMPRESSION7_3_H
#ifdef __cplusplus
extern "C"{
#endif

typedef enum {
    BLACK = 0,
    WHITE = 1,
    GREEN = 2,
    BLUE = 3,
    RED = 4,
    YELLOW = 5,
    ORANGE = 6,
    CLEAN = 7,

}  COLOR;

#define DISP_WIDTH 800
#define DISP_HEIGHT 480

    void inky_set_pixel(unsigned int x, unsigned int y, COLOR color);
    void inky_init_hw(void);
    void inky_show(void);
    void inky_set_border(COLOR);

#ifdef __cplusplus
}
#endif
#endif //ESP32_WEATHERSTATION_INKY_IMPRESSION7_3_H
