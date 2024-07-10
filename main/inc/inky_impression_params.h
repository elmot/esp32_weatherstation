//
// Created by elmot on 10 Jul 2024.
//

#ifndef ESP32_WEATHERSTATION_INKY_IMPRESSION_PARAMS_H
#define ESP32_WEATHERSTATION_INKY_IMPRESSION_PARAMS_H

#define DISP_PIN_BUSY 3
#define DISP_PIN_RST  4
#define DISP_PIN_DC   5

#define DISP_PIN_MOSI 6
#define DISP_PIN_CLK  7
#define DISP_PIN_CS   10

#define DISP_SPI_HOST    SPI2_HOST

#define DISP_BAUD         (10 * 1000 * 1000)

#endif //ESP32_WEATHERSTATION_INKY_IMPRESSION_PARAMS_H
