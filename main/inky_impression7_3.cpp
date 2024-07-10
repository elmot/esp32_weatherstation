#include "sys/types.h"

#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <cstring>

#include "inky_impression7_3.h"
#include "inky_impression_params.h"

#define DISP_BUF_SIZE     (DISP_HEIGHT * DISP_WIDTH / 2)

DMA_ATTR uint8_t buffer[DISP_BUF_SIZE];
spi_device_handle_t inky;
static COLOR borderColor;

void inky_set_pixel(uint x, uint y, COLOR color) {
    uint index = ((y % DISP_HEIGHT) * DISP_WIDTH + (x % DISP_WIDTH)) / 2;
    if (x & 1) { buffer[index] = (buffer[index] & 0x7) | ((color << 4) & 0x70); }
    else { buffer[index] = (buffer[index] & 0x70) | (color & 0x7); }
}

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

void spi_write(uint32_t dc, const uint8_t *values, size_t len) {
    esp_err_t ret;
    gpio_set_level(static_cast<gpio_num_t>(DISP_PIN_DC), dc);
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
    if (gpio_get_level(static_cast<gpio_num_t>(DISP_PIN_BUSY))) {
        printf("Busy Wait: Held high. Waiting for %dms", timeoutMs);
    }

// If the busy_pin is *low* (pulled down by inky)
// then wait for it to high.
    vTaskDelay(pdMS_TO_TICKS(timeoutMs));
    if (!gpio_get_level((gpio_num_t) DISP_PIN_BUSY)) {
        printf("Busy Wait: Timed out after %dms", timeoutMs);
    }
}

void inky_setup() {

    gpio_set_level((gpio_num_t) DISP_PIN_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level((gpio_num_t) DISP_PIN_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level((gpio_num_t) DISP_PIN_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level((gpio_num_t) DISP_PIN_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(100));

    busy_wait(1000);

    send_command(AC073TC1_CMDH);
    DMA_ATTR static const uint8_t cmdh_data[] = {0x49, 0x55, 0x20, 0x08, 0x09, 0x18};
    send_data(cmdh_data, sizeof(cmdh_data));

    send_command(AC073TC1_PWR);
    DMA_ATTR static const uint8_t pwr_data[] = {0x3F, 0x00, 0x32, 0x2A, 0x0E, 0x2A};
    send_data(pwr_data, sizeof(pwr_data));

    send_command(AC073TC1_PSR);
    DMA_ATTR static const uint8_t psr_data[] = {0x5F, 0x69};
    send_data(psr_data, sizeof(psr_data));

    send_command(AC073TC1_POFS);
    DMA_ATTR static const uint8_t pofs_data[] = {0x00, 0x54, 0x00, 0x44};
    send_data(pofs_data, sizeof(pofs_data));

    send_command(AC073TC1_BTST1);
    DMA_ATTR static const uint8_t btst1_data[] = {0x40, 0x1F, 0x1F, 0x2C};
    send_data(btst1_data, sizeof(btst1_data));

    send_command(AC073TC1_BTST2);
    DMA_ATTR static const uint8_t btst2_data[] = {0x6F, 0x1F, 0x16, 0x25};
    send_data(btst2_data, sizeof(btst2_data));

    send_command(AC073TC1_BTST3);
    DMA_ATTR static const uint8_t btst3_data[] = {0x6F, 0x1F, 0x1F, 0x22};
    send_data(btst3_data, sizeof(btst3_data));

    send_command(AC073TC1_IPC);
    DMA_ATTR static const uint8_t ipc_data[] = {0x00, 0x04};
    send_data(ipc_data, sizeof(ipc_data));

    send_command(AC073TC1_PLL);
    DMA_ATTR static const uint8_t pll_data[] = {0x02};
    send_data(pll_data, sizeof(pll_data));

    send_command(AC073TC1_TSE);
    DMA_ATTR static const uint8_t tse_data[] = {0x00};
    send_data(tse_data, sizeof(tse_data));

    send_command(AC073TC1_CDI);
    DMA_ATTR static uint8_t cdi_data = 0x1F | (unsigned char) (borderColor << 5);
    send_data(&cdi_data, 1);

    send_command(AC073TC1_TCON);
    DMA_ATTR static const uint8_t tcon_data[] = {0x02, 0x00};
    send_data(tcon_data, sizeof(tcon_data));

    send_command(AC073TC1_TRES);
    DMA_ATTR static const uint8_t tres_data[] = {0x03, 0x20, 0x01, 0xE0};
    send_data(tres_data, sizeof(tres_data));

    send_command(AC073TC1_VDCS);
    DMA_ATTR static const uint8_t vdcs_data[] = {0x1E};
    send_data(vdcs_data, sizeof(vdcs_data));

    send_command(AC073TC1_T_VDCS);
    DMA_ATTR static const uint8_t tvdcs_data[] = {0x00};
    send_data(tvdcs_data, sizeof(tvdcs_data));

    send_command(AC073TC1_AGID);
    DMA_ATTR static const uint8_t agid_data[] = {0x00};
    send_data(agid_data, sizeof(agid_data));

    send_command(AC073TC1_PWS);
    DMA_ATTR static const uint8_t pws_data[] = {0x2F};
    send_data(pws_data, sizeof(pws_data));

    send_command(AC073TC1_CCSET);
    DMA_ATTR static const uint8_t ccset_data[] = {0x00};
    send_data(ccset_data, sizeof(ccset_data));

    send_command(AC073TC1_TSSET);
    DMA_ATTR static const uint8_t tsset_data[] = {0x00};
    send_data(tsset_data, sizeof(tsset_data));
}

void hw_update() {
    send_command(AC073TC1_PON);
    busy_wait(400);
    DMA_ATTR static const uint8_t zero = 0;

    send_command(AC073TC1_DRF);
    send_data((uint8_t *) &zero, 1);
    busy_wait(45000);

    send_command(AC073TC1_POF);
    send_data((uint8_t *) &zero, 1);

    busy_wait(400);
}

void inky_init_hw(void) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
    static const gpio_config_t gpioCfgOut = {
            .pin_bit_mask = (1ULL << DISP_PIN_RST) | (1ULL << DISP_PIN_DC),
            .mode =GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
    };
    static const gpio_config_t gpioCfgIn = {
            .pin_bit_mask = (1ULL << DISP_PIN_BUSY),
            .mode =GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,

    };
    static const spi_bus_config_t buscfg = {
            .mosi_io_num = static_cast<gpio_num_t>(DISP_PIN_MOSI),
            .sclk_io_num = static_cast<gpio_num_t>(DISP_PIN_CLK),
            .max_transfer_sz = DISP_BUF_SIZE,
    };
    static const spi_device_interface_config_t devcfg = {
            .mode = 0,
            .clock_speed_hz = DISP_BAUD,            //Clock out at 10 MHz
            .spics_io_num = DISP_PIN_CS,            //CS pin
            .queue_size = 200,
    };
#pragma GCC diagnostic pop

    esp_err_t ret = gpio_config(&gpioCfgOut);
    ESP_ERROR_CHECK(ret);

    ret = gpio_config(&gpioCfgIn);
    ESP_ERROR_CHECK(ret);
    ret = gpio_set_level(static_cast<gpio_num_t>(DISP_PIN_RST), 1);
    ESP_ERROR_CHECK(ret);
    ret = gpio_set_level(static_cast<gpio_num_t>(DISP_PIN_CS), 1);
    ESP_ERROR_CHECK(ret);

    //Initialize the SPI bus
    ret = spi_bus_initialize(DISP_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(ret);
    //Attach the LCD to the SPI bus
    ret = spi_bus_add_device(DISP_SPI_HOST, &devcfg, &inky);
    ESP_ERROR_CHECK(ret);
}


/** Show buffer on display.
 *
 */
extern "C" void inky_show() {
    inky_setup();

    send_command(AC073TC1_DTM);
    for (size_t i = 0; i < DISP_BUF_SIZE; i += 1000)
        send_data(buffer + i, 1000);
    hw_update();
}

void inky_set_border(COLOR color) {
    borderColor = color;
}
