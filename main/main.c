#include <stdio.h>
#include <driver/gpio.h>
#include <driver/spi_master.h>

#include "ac073tc1.h"

#define LCD_HOST    SPI2_HOST

//DC_PIN = 5
//RESET_PIN = 4
//BUSY_PIN = 3
//
//MOSI_PIN = 6
//SCLK_PIN = 7
//CS0_PIN = 10

#define PIN_NUM_DISP_MOSI 6
#define PIN_NUM_DISP_CLK  7
#define PIN_NUM_DISP_CS   10

#define PIN_NUM_DISP_DC   5
#define PIN_NUM_DISP_RST  4
#define PIN_NUM_DISP_BUSY 3

#define DISP_HEIGHT       480
#define DISP_WIDTH        800
#define DISP_BAUD         1000000
#define DISP_BUF_SIZE     (DISP_HEIGHT * DISP_WIDTH / 2)

uint8_t buffer[DISP_BUF_SIZE];

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
    vTaskDelay(pdMS_TO_TICKS(timeoutMs));//todo not necessarily to wait all the time
    if (!gpio_get_level(PIN_NUM_DISP_BUSY)) {
        printf("Busy Wait: Timed out after %dms", timeoutMs);
    }
}

void inky_send_command(spi_device_handle_t spiDev, uint8_t command, const uint8_t *txData, size_t data_len) {
    static spi_transaction_t transaction = {0, 0, 0, 0, 0, 0, {NULL}, {NULL}};
    gpio_set_level(PIN_NUM_DISP_DC, 0);
    transaction.length = 1;
    transaction.tx_buffer = &command;
    esp_err_t ret = spi_device_polling_transmit(spiDev, &transaction);
    ESP_ERROR_CHECK(ret);
    if (txData != NULL) {
        gpio_set_level(PIN_NUM_DISP_DC, 1);
        transaction.length = data_len;
        transaction.tx_buffer = txData;
        ret = spi_device_polling_transmit(spiDev, &transaction);
        ESP_ERROR_CHECK(ret);
    }
}

spi_device_handle_t hw_init() {
    //Initialize non-SPI GPIOs
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = 1ULL << PIN_NUM_DISP_BUSY;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = false;
    gpio_config(&io_conf);

    io_conf.pin_bit_mask = ((1ULL << PIN_NUM_DISP_DC) | (1ULL << PIN_NUM_DISP_RST));
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = true;
    gpio_config(&io_conf);

    esp_err_t ret;
    spi_device_handle_t inkySpi;
    spi_bus_config_t busCfg = {
            .miso_io_num = -1,
            .mosi_io_num = PIN_NUM_DISP_MOSI,
            .sclk_io_num = PIN_NUM_DISP_CLK,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = 0
    };
    spi_device_interface_config_t devCfg = {
            .clock_speed_hz = DISP_BAUD,     //Clock out at
            .mode = 0,                              //SPI mode 0
            .spics_io_num = PIN_NUM_DISP_CS,             //CS pin
            .queue_size = 7,                        //We want to be able to queue 7 transactions at a time
//            .pre_cb = lcd_spi_pre_transfer_callback, //Specify pre-transfer callback to handle D/C line
    };
    //Initialize the SPI bus
    ret = spi_bus_initialize(LCD_HOST, &busCfg, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(ret);
    //Attach the Inky to the SPI bus
    ret = spi_bus_add_device(LCD_HOST, &devCfg, &inkySpi);
    ESP_ERROR_CHECK(ret);
    //Initialize the Inky
    gpio_set_level(PIN_NUM_DISP_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level(PIN_NUM_DISP_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level(PIN_NUM_DISP_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level(PIN_NUM_DISP_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(100));

    busy_wait(1000);
    return inkySpi;
}

void inky_init(spi_device_handle_t spi) {
    uint8_t cmdh_data[] = {0x49, 0x55, 0x20, 0x08, 0x09, 0x18};
    inky_send_command(spi, AC073TC1_CMDH, cmdh_data, sizeof(cmdh_data));

    uint8_t pwr_data[] = {0x3F, 0x00, 0x32, 0x2A, 0x0E, 0x2A};
    inky_send_command(spi, AC073TC1_PWR, pwr_data, sizeof(pwr_data));

    uint8_t psr_data[] = {0x5F, 0x69};
    inky_send_command(spi, AC073TC1_PSR, psr_data, sizeof(psr_data));

    uint8_t pofs_data[] = {0x00, 0x54, 0x00, 0x44};
    inky_send_command(spi, AC073TC1_POFS, pofs_data, sizeof(pofs_data));

    uint8_t btst1_data[] = {0x40, 0x1F, 0x1F, 0x2C};
    inky_send_command(spi, AC073TC1_BTST1, btst1_data, sizeof(btst1_data));

    uint8_t btst2_data[] = {0x6F, 0x1F, 0x16, 0x25};
    inky_send_command(spi, AC073TC1_BTST2, btst2_data, sizeof(btst2_data));

    uint8_t btst3_data[] = {0x6F, 0x1F, 0x1F, 0x22};
    inky_send_command(spi, AC073TC1_BTST3, btst3_data, sizeof(btst3_data));

    uint8_t ipc_data[] = {0x00, 0x04};
    inky_send_command(spi, AC073TC1_IPC, ipc_data, sizeof(ipc_data));

    uint8_t pll_data[] = {0x02};
    inky_send_command(spi, AC073TC1_PLL, pll_data, sizeof(pll_data));

    uint8_t tse_data[] = {0x00};
    inky_send_command(spi, AC073TC1_TSE, tse_data, sizeof(tse_data));

    uint8_t cdi_data[] = {0x3F};
    inky_send_command(spi, AC073TC1_CDI, cdi_data, sizeof(cdi_data));

    uint8_t tcon_data[] = {0x02, 0x00};
    inky_send_command(spi, AC073TC1_TCON, tcon_data, sizeof(tcon_data));

    uint8_t tres_data[] = {0x03, 0x20, 0x01, 0xE0};
    inky_send_command(spi, AC073TC1_TRES, tres_data, sizeof(tres_data));

    uint8_t vdcs_data[] = {0x1E};
    inky_send_command(spi, AC073TC1_VDCS, vdcs_data, sizeof(vdcs_data));

    uint8_t tvdcs_data[] = {0x00};
    inky_send_command(spi, AC073TC1_T_VDCS, tvdcs_data, sizeof(tvdcs_data));

    uint8_t agid_data[] = {0x00};
    inky_send_command(spi, AC073TC1_AGID, agid_data, sizeof(agid_data));

    uint8_t pws_data[] = {0x2F};
    inky_send_command(spi, AC073TC1_PWS, pws_data, sizeof(pws_data));

    uint8_t ccset_data[] = {0x00};
    inky_send_command(spi, AC073TC1_CCSET, ccset_data, sizeof(ccset_data));

    uint8_t tsset_data[] = {0x00};
    inky_send_command(spi, AC073TC1_TSSET, tsset_data, sizeof(tsset_data));

}

void hw_update(spi_device_handle_t spi) {
    inky_send_command(spi, AC073TC1_PON, NULL, 0);
    busy_wait(400);

    inky_send_command(spi, AC073TC1_DRF, (uint8_t *) {0x00}, 1);
    busy_wait(45000);

    inky_send_command(spi, AC073TC1_POF, (uint8_t *) {0x00}, 1);

    busy_wait(400);
}

void inky_show(spi_device_handle_t inkySpi) {
//    inky_init();

    inky_send_command(inkySpi, AC073TC1_DTM, buffer, BUFSIZ);
    hw_update(inkySpi);
}

void set_pixel(unsigned int x, unsigned int y, enum COLOR v) {
    x = x % DISP_WIDTH;
    y = y % DISP_HEIGHT;
    size_t index = (y * WIDTH + x) / 2;
    if (x % 1) {
        buffer[index] = (buffer[index] & 0x7) | ((v << 4) & 0x70);
    } else {
        buffer[index] = (buffer[index] & 0x70) | (v & 0x7);
    }
}

__attribute__((noreturn)) void app_main() {
    (void) app_main;

    spi_device_handle_t inkySpi = hw_init();
    inky_init(inkySpi);
    for (int x = 0; x < WIDTH; ++x) {
        for (int y = 0; y < HEIGHT; ++y) {
            set_pixel(x, y, (3 + y / 100 + x / 110) % 8);
        }

    }
    inky_show(inkySpi);

    while (1) {
        printf("end loop\n\r");
        vTaskDelay(100);
    }
}
