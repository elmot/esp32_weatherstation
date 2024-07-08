//
// Created by elmot on 2 Jul 2024.
//

#ifndef RPI_PICO_WEATHER_STATION_AC073TC1_H
#define RPI_PICO_WEATHER_STATION_AC073TC1_H
#define AC073TC1_CMDH 0xAA
#define AC073TC1_PWR 0x01
#define AC073TC1_PSR 0x00
#define AC073TC1_POFS 0x03
#define AC073TC1_BTST1 0x05
#define AC073TC1_BTST2 0x06
#define AC073TC1_BTST3 0x08
#define AC073TC1_IPC 0x13
#define AC073TC1_PLL 0x30
#define AC073TC1_TSE 0x41
#define AC073TC1_CDI 0x50
#define AC073TC1_TCON 0x60
#define AC073TC1_TRES 0x61
#define AC073TC1_VDCS 0x82
#define AC073TC1_T_VDCS 0x84
#define AC073TC1_AGID 0x86
#define AC073TC1_PWS 0xE3
#define AC073TC1_CCSET 0xE0
#define AC073TC1_TSSET 0xE6
#define AC073TC1_PON 0x04
#define AC073TC1_DRF 0x12
#define AC073TC1_POF 0x02
#define AC073TC1_DTM 0x10

#define WIDTH 800
#define HEIGHT 480

enum COLOR {
    BLACK = 0,
    WHITE = 1,
    GREEN = 2,
    BLUE = 3,
    RED = 4,
    YELLOW = 5,
    ORANGE = 6,
    CLEAN = 7,

};
#endif //RPI_PICO_WEATHER_STATION_AC073TC1_H