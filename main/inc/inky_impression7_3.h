//
// Created by elmot on 10 Jul 2024.
//

#ifndef ESP32_WEATHERSTATION_INKY_IMPRESSION7_3_H
#define ESP32_WEATHERSTATION_INKY_IMPRESSION7_3_H
#ifdef __cplusplus
extern "C" {
#endif

class InkyImpression7_3f {
public :
    typedef enum {
        BLACK = 0,
        WHITE = 1,
        GREEN = 2,
        BLUE = 3,
        RED = 4,
        YELLOW = 5,
        ORANGE = 6,
        CLEAN = 7,

    } COLOR;
    static constexpr unsigned int WIDTH = 800;
    static constexpr unsigned int HEIGHT = 480;
    static constexpr size_t BUF_SIZE = (WIDTH * HEIGHT / 2);
    static void set_pixel(unsigned int x, unsigned int y, COLOR color);
    static void init_hw(void);
    static void show(void);
    static void set_border(COLOR);
    static void set_background(const uint8_t *packet_pixels) ;

};



#ifdef __cplusplus
}
#endif
#endif //ESP32_WEATHERSTATION_INKY_IMPRESSION7_3_H
