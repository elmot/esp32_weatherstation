//
// Created by elmot on 10 Jul 2024.
//

#ifndef ESP32_WEATHERSTATION_INKY_IMPRESSION7_3_H
#define ESP32_WEATHERSTATION_INKY_IMPRESSION7_3_H
#ifndef __cplusplus
#error Only C++ issupported
#endif

#include <driver/spi_master.h>
class InkyImpression7_3f {
public :
    InkyImpression7_3f();

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

    InkyImpression7_3f(COLOR borderColor, unsigned char *buffer);

    void set_pixel(unsigned int x, unsigned int y, COLOR color);
    void init_hw();
    void show();
    void set_border(COLOR);
    void set_background(const uint8_t *packet_pixels) ;
private:

    spi_device_handle_t spi{};

    COLOR borderColor = BLACK;

    void setup();

    unsigned char *buffer;

    void spi_write(uint32_t dc, const uint8_t *values, size_t len);

    void send_command(uint8_t command);

    void send_data(const uint8_t *data, size_t len);

    void hw_update();
};

#endif //ESP32_WEATHERSTATION_INKY_IMPRESSION7_3_H
