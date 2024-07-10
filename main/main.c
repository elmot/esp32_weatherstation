#include <stdio.h>
#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <string.h>
#include <sys/types.h>

#include "ac073tc1.h"

#define LCD_HOST    SPI2_HOST

//DC_PIN = 5
//RESET_PIN = 4
//BUSY_PIN = 3
//
//MOSI_PIN = 6
//SCLK_PIN = 7
//CS0_PIN = 10

#define PIN_NUM_DISP_BUSY 3
#define PIN_NUM_DISP_RST  4
#define PIN_NUM_DISP_DC   5

#define PIN_NUM_DISP_MOSI 6
#define PIN_NUM_DISP_CLK  7
#define PIN_NUM_DISP_CS   10

#define DISP_WIDTH 800
#define DISP_HEIGHT 480
#define DISP_BAUD         (10 * 1000 * 1000)
#define DISP_BUF_SIZE     (DISP_HEIGHT * DISP_WIDTH / 2)

DMA_ATTR uint8_t buffer[DISP_BUF_SIZE];
spi_device_handle_t inky;

void set_pixel(uint x, uint y, uint8_t color) {
    uint index = ((y % DISP_HEIGHT) * DISP_WIDTH + (x % DISP_WIDTH)) / 2;
    if (x & 1) { buffer[index] = (buffer[index] & 0x7) | ((color << 4) & 0x70); }
    else { buffer[index] = (buffer[index] & 0x70) | (color & 0x7); }
}

void init_hw(void) {
    static const gpio_config_t gpioCfgOut = {
            .pin_bit_mask = (1ULL << PIN_NUM_DISP_RST) | (1ULL << PIN_NUM_DISP_DC),
            .mode =GPIO_MODE_OUTPUT,
            .intr_type = GPIO_INTR_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .pull_up_en = GPIO_PULLUP_DISABLE,

    };
    esp_err_t ret = gpio_config(&gpioCfgOut);
    ESP_ERROR_CHECK(ret);

    static const gpio_config_t gpioCfgIn = {
            .pin_bit_mask = (1ULL << PIN_NUM_DISP_BUSY),
            .mode =GPIO_MODE_INPUT,
            .intr_type = GPIO_INTR_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .pull_up_en = GPIO_PULLUP_DISABLE,

    };
    ret = gpio_config(&gpioCfgIn);
    ESP_ERROR_CHECK(ret);
    ret = gpio_set_level(PIN_NUM_DISP_RST, 1);
    ESP_ERROR_CHECK(ret);
    ret = gpio_set_level(PIN_NUM_DISP_CS, 1);
    ESP_ERROR_CHECK(ret);


    spi_bus_config_t buscfg = {
            .miso_io_num = -1,
            .mosi_io_num = PIN_NUM_DISP_MOSI,
            .sclk_io_num = PIN_NUM_DISP_CLK,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = DISP_BUF_SIZE
    };
    spi_device_interface_config_t devcfg = {
            .clock_speed_hz = DISP_BAUD,     //Clock out at 10 MHz
            .mode = 0,                              //SPI mode 0
            .spics_io_num = PIN_NUM_DISP_CS,             //CS pin
            .queue_size = 200,                        //We want to be able to queue 7 transactions at a time
    };
    //Initialize the SPI bus
    ret = spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(ret);
    //Attach the LCD to the SPI bus
    ret = spi_bus_add_device(LCD_HOST, &devcfg, &inky);
    ESP_ERROR_CHECK(ret);
}


void spi_write(uint32_t dc, const uint8_t *values, size_t len) {
    esp_err_t ret;
    gpio_set_level(PIN_NUM_DISP_DC, dc);
    spi_transaction_t transaction;
    memset(&transaction, 0, sizeof(transaction));
    transaction.length = len * 8;
    transaction.tx_buffer = values;
    ret = spi_device_polling_transmit(inky, &transaction);
    ESP_ERROR_CHECK(ret);

}

void send_command(uint8_t command) {
    spi_write(0, &command, 1);
}

void send_data(const uint8_t *data, size_t len) {
    spi_write(1, data, len);
}

/**
 *     Wait for busy/wait pin
 * @param timeout
 */
void busy_wait(unsigned int timeoutMs) {
// If the busy_pin is *high* (pulled up by host)
// then assume we're not getting a signal from inky
// and wait the timeout period to be safe.
    if (gpio_get_level(PIN_NUM_DISP_BUSY)) {
        printf("Busy Wait: Held high. Waiting for %dms", timeoutMs);
    }

// If the busy_pin is *low* (pulled down by inky)
// then wait for it to high.
    vTaskDelay(pdMS_TO_TICKS(timeoutMs));
    if (!gpio_get_level(PIN_NUM_DISP_BUSY)) {
        printf("Busy Wait: Timed out after %dms", timeoutMs);
    }
}

void inky_setup() {

    gpio_set_level(PIN_NUM_DISP_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level(PIN_NUM_DISP_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level(PIN_NUM_DISP_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level(PIN_NUM_DISP_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(100));

    busy_wait(1000);

    send_command(AC073TC1_CMDH);
    uint8_t cmdh_data[] = {0x49, 0x55, 0x20, 0x08, 0x09, 0x18};
    send_data(cmdh_data, sizeof(cmdh_data));

    send_command(AC073TC1_PWR);
    uint8_t pwr_data[] = {0x3F, 0x00, 0x32, 0x2A, 0x0E, 0x2A};
    send_data(pwr_data, sizeof(pwr_data));

    send_command(AC073TC1_PSR);
    uint8_t psr_data[] = {0x5F, 0x69};
    send_data(psr_data, sizeof(psr_data));

    send_command(AC073TC1_POFS);
    uint8_t pofs_data[] = {0x00, 0x54, 0x00, 0x44};
    send_data(pofs_data, sizeof(pofs_data));

    send_command(AC073TC1_BTST1);
    uint8_t btst1_data[] = {0x40, 0x1F, 0x1F, 0x2C};
    send_data(btst1_data, sizeof(btst1_data));

    send_command(AC073TC1_BTST2);
    uint8_t btst2_data[] = {0x6F, 0x1F, 0x16, 0x25};
    send_data(btst2_data, sizeof(btst2_data));

    send_command(AC073TC1_BTST3);
    uint8_t btst3_data[] = {0x6F, 0x1F, 0x1F, 0x22};
    send_data(btst3_data, sizeof(btst3_data));

    send_command(AC073TC1_IPC);
    uint8_t ipc_data[] = {0x00, 0x04};
    send_data(ipc_data, sizeof(ipc_data));

    send_command(AC073TC1_PLL);
    uint8_t pll_data[] = {0x02};
    send_data(pll_data, sizeof(pll_data));

    send_command(AC073TC1_TSE);
    uint8_t tse_data[] = {0x00};
    send_data(tse_data, sizeof(tse_data));

    send_command(AC073TC1_CDI);
    uint8_t cdi_data[] = {0x3F};
    send_data(cdi_data, sizeof(cdi_data));

    send_command(AC073TC1_TCON);
    uint8_t tcon_data[] = {0x02, 0x00};
    send_data(tcon_data, sizeof(tcon_data));

    send_command(AC073TC1_TRES);
    uint8_t tres_data[] = {0x03, 0x20, 0x01, 0xE0};
    send_data(tres_data, sizeof(tres_data));

    send_command(AC073TC1_VDCS);
    uint8_t vdcs_data[] = {0x1E};
    send_data(vdcs_data, sizeof(vdcs_data));

    send_command(AC073TC1_T_VDCS);
    uint8_t tvdcs_data[] = {0x00};
    send_data(tvdcs_data, sizeof(tvdcs_data));

    send_command(AC073TC1_AGID);
    uint8_t agid_data[] = {0x00};
    send_data(agid_data, sizeof(agid_data));

    send_command(AC073TC1_PWS);
    uint8_t pws_data[] = {0x2F};
    send_data(pws_data, sizeof(pws_data));

    send_command(AC073TC1_CCSET);
    uint8_t ccset_data[] = {0x00};
    send_data(ccset_data, sizeof(ccset_data));

    send_command(AC073TC1_TSSET);
    uint8_t tsset_data[] = {0x00};
    send_data(tsset_data, sizeof(tsset_data));
}

void hw_update() {
    send_command(AC073TC1_PON);
    busy_wait(400);

    send_command(AC073TC1_DRF);
    send_data((uint8_t *) {0x00}, 1);
    busy_wait(45000);

    send_command(AC073TC1_POF);
    send_data((uint8_t *) {0x00}, 1);

    busy_wait(400);
}

/** Show buffer on display.
 *
 */
void inky_show() {
    inky_setup();

    send_command(AC073TC1_DTM);
    for (size_t i = 0; i < DISP_BUF_SIZE; i += 1000)
        send_data(buffer + i, 1000);
    hw_update();
}

__attribute__((noreturn)) void app_main() {
    (void) app_main;

    init_hw();
    for (int y = 0; y < DISP_HEIGHT; ++y) {
        for (int x = 0; x < DISP_WIDTH; ++x) {
            set_pixel(x, y, (3 + y / 100 + x / 110) % 8);
        }
    }
    puts("end write");
    inky_show();
    while (1) {
        puts("End_loop");
        vTaskDelay(50);
    }

}
