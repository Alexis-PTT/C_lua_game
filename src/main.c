//
// #include <string.h>
// #include "freertos/FreeRTOS.h"
// #include "driver/i2c_master.h"
// #include "esp_lcd_io_i2c.h"
// #include "esp_lcd_panel_ops.h"
// #include "esp_lcd_panel_io.h"
// #include "esp_lcd_panel_sh1106.h"
//
// #define I2C_HOST I2C_NUM_0
//
// #ifdef __cplusplus
// extern "C"
// #endif
// void app_main(void)
// {
//     /* I2C CONFIGURATION */
//
//     // i2c bus configuration
//     i2c_master_bus_config_t bus_config = {
//         .i2c_port = I2C_HOST,               // I2C port number
//         .sda_io_num = 8,  // GPIO number for I2C sda signal, set from "idf menuconfig"
//         .scl_io_num = 9,  // GPIO number for I2C scl signal, set from "idf menuconfig"
//         .clk_source = I2C_CLK_SRC_DEFAULT,  // I2C clock source, just use the default
//         .glitch_ignore_cnt = 7,             // glitch filter, again, just use the default
//         .intr_priority = 0,                 // interrupt priority, default to 0
//         .trans_queue_depth = 0,             // transaction queue depth, default to 0
//         .flags = {
//             .enable_internal_pullup = true, // enable internal pullup resistors (oled screen does not have one)
//             .allow_pd = false,              // just using the default value
//         },
//     };
//
//     // Create the i2c bus handle
//     i2c_master_bus_handle_t i2c_bus_handle = NULL;
//     ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &i2c_bus_handle));
//
//     // Create the i2c io handle
//     esp_lcd_panel_io_handle_t io_handle = NULL;
//     esp_lcd_panel_io_i2c_config_t io_config = ESP_SH1106_DEFAULT_IO_CONFIG;
//     ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(i2c_bus_handle, &io_config, &io_handle));
//
//
//     /* SCREEN CONFIGURATION */
//
//     // sh1106 panel configuration (most of the values are not used, but must be set to avoid cpp warnings)
//     esp_lcd_panel_dev_config_t panel_config = {
//         .reset_gpio_num = -1,                         // sh1106 does not have a reset pin, so set to -1
//         .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,   // not even used, but must be set to avoid cpp warnings
//         .data_endian = LCD_RGB_DATA_ENDIAN_LITTLE,    // not even used, but must be set to avoid cpp warnings
//         .bits_per_pixel = SH1106_PIXELS_PER_BYTE / 8, // bpp = 1 (monochrome, that's important)
//         .flags = {
//             .reset_active_high = false,               // not even used, but must be set to avoid cpp warnings
//         },
//         .vendor_config = NULL,                        // no need for custom vendor config, not implemented
//     };
//
//     // Create the panel handle from the sh1106 driver
//     esp_lcd_panel_handle_t panel_handle = NULL;
//     ESP_ERROR_CHECK(esp_lcd_new_panel_sh1106(io_handle, &panel_config, &panel_handle));
//
//     // Reset the screen (no reset pin, so it's a no-op here, optional)
//     ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
//
//     // Initialize the screen (this one isn't optional at all!)
//     ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
//
//     // Turn on the screen (Easier to see something, right?)
//     ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
//
//
//     /* SCREEN PIXEL TEST */
//
//     // Create a buffer to hold the screen data
//     uint8_t buffer_data[SH1106_BUFFER_SIZE];
//     memset(buffer_data, 0, SH1106_BUFFER_SIZE);
//
//     // Just turn on the first top-left pixel and the last bottom-right pixel to show individual pixels control
//     // NOTE : Refer to driver README.md file for more information about the screen buffer format
//     buffer_data[0] = 0b00000001;
//     buffer_data[SH1106_BUFFER_SIZE - 1] = 0b10000000;
//
//     // Send the buffer to the screen
//     ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, SH1106_WIDTH, SH1106_HEIGHT, buffer_data));
//     vTaskDelay(pdMS_TO_TICKS(2000)); // wait a bit
//
//
//     // turn all the pixels on
//     memset(buffer_data, 0xFF, SH1106_BUFFER_SIZE);
//     // only update a portion of the screen to show that partial updates work
//     ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(panel_handle, 48, 16, 80, 48, buffer_data));
//     vTaskDelay(pdMS_TO_TICKS(2000)); // wait a bit
//
//
//     // turn all the pixels white using a sliding window and small buffer
//     memset(buffer_data, 0x00, SH1106_BUFFER_SIZE);
//     memset(buffer_data, 0xFF, 8); // only turn on 8x8 pixels at a time to show that small updates work
//     for (int y = 0; y < SH1106_HEIGHT; y += 8) {
//         for (int x = 0; x < SH1106_WIDTH; x += 8) {
//             ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(panel_handle, x, y, x + 8, y + 8, buffer_data));
//             vTaskDelay(pdMS_TO_TICKS(50)); // add a small delay to make the sliding window effect visible
//         }
//     }
// }

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "screen.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

// static const char *TAG = "tft_test";
//
// // ============================================================
// // PIN CONFIGURATION — adjust to match your wiring
// // ============================================================
// #define PIN_MOSI        6
// #define PIN_CLK         4
// #define PIN_CS          7
// #define PIN_DC          2
// #define PIN_RST         -1
// #define PIN_BL          -1
//
// // ============================================================
// // DISPLAY CONFIGURATION
// // ============================================================
// #define LCD_H_RES       240
// #define LCD_V_RES       320
// #define LCD_HOST        SPI2_HOST
//
// // ============================================================
// // DRIVER SELECTION
// // Switch between ST7789 and ILI9341 here:
// //   #define USE_ST7789
// //   #define USE_ILI9341
// // ============================================================
// #define USE_ST7789
// // #define USE_ILI9341
//
//
// // ============================================================
// // Simple color fill helper
// // ============================================================
// static void fill_color(esp_lcd_panel_handle_t panel, uint16_t color)
// {
//     // Allocate a single row buffer
//     uint16_t *line = heap_caps_malloc(LCD_H_RES * sizeof(uint16_t), MALLOC_CAP_DMA);
//     if (!line) {
//         ESP_LOGE(TAG, "Failed to alloc line buffer");
//         return;
//     }
//     for (int i = 0; i < LCD_H_RES; i++) {
//         // Swap bytes for little-endian SPI transfer
//         line[i] = (color >> 8) | (color << 8);
//     }
//     for (int y = 0; y < LCD_V_RES; y++) {
//         esp_lcd_panel_draw_bitmap(panel, 0, y, LCD_H_RES, y + 1, line);
//     }
//     free(line);
// }
//
// // ============================================================
// // Draw a simple test pattern:
// //   Top third    = RED
// //   Middle third = GREEN
// //   Bottom third = BLUE
// // ============================================================
// static void draw_test_pattern(esp_lcd_panel_handle_t panel)
// {
//     uint16_t colors[3] = {
//         0xF800,  // RED   (RGB565)
//         0x07E0,  // GREEN
//         0x001F,  // BLUE
//     };
//
//     uint16_t *line = heap_caps_malloc(LCD_H_RES * sizeof(uint16_t), MALLOC_CAP_DMA);
//     if (!line) {
//         ESP_LOGE(TAG, "Failed to alloc line buffer");
//         return;
//     }
//
//     int section = LCD_V_RES / 3;
//     for (int y = 0; y < LCD_V_RES; y++) {
//         uint16_t color = colors[y / section < 3 ? y / section : 2];
//         uint16_t swapped = (color >> 8) | (color << 8);
//         for (int x = 0; x < LCD_H_RES; x++) {
//             line[x] = swapped;
//         }
//         esp_lcd_panel_draw_bitmap(panel, 0, y, LCD_H_RES, y + 1, line);
//     }
//     free(line);
// }
//
// void app_main(void)
// {
//     // --------------------------------------------------------
//     // Backlight ON
//     // --------------------------------------------------------
//     if (PIN_BL >= 0) {
//         gpio_set_direction(PIN_BL, GPIO_MODE_OUTPUT);
//         gpio_set_level(PIN_BL, 1);
//     }
//
//     // --------------------------------------------------------
//     // SPI bus init
//     // --------------------------------------------------------
//     spi_bus_config_t bus_cfg = {
//         .mosi_io_num     = PIN_MOSI,
//         .miso_io_num     = -1,       // not needed for display
//         .sclk_io_num     = PIN_CLK,
//         .quadwp_io_num   = -1,
//         .quadhd_io_num   = -1,
//         .max_transfer_sz = LCD_H_RES * 80 * sizeof(uint16_t),
//     };
//     ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &bus_cfg, SPI_DMA_CH_AUTO));
//     ESP_LOGI(TAG, "SPI bus initialised");
//
//     // --------------------------------------------------------
//     // Panel IO (SPI → LCD)
//     // --------------------------------------------------------
//     esp_lcd_panel_io_handle_t io_handle = NULL;
//     esp_lcd_panel_io_spi_config_t io_cfg = {
//         .dc_gpio_num       = PIN_DC,
//         .cs_gpio_num       = PIN_CS,
//         .pclk_hz           = 40 * 1000 * 1000,   // 40 MHz
//         .lcd_cmd_bits      = 8,
//         .lcd_param_bits    = 8,
//         .spi_mode          = 0,
//         .trans_queue_depth = 10,
//     };
//     ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST,
//                                               &io_cfg, &io_handle));
//     ESP_LOGI(TAG, "Panel IO created");
//
//     // --------------------------------------------------------
//     // Panel device init
//     // --------------------------------------------------------
//     esp_lcd_panel_handle_t panel_handle = NULL;
//     esp_lcd_panel_dev_config_t panel_cfg = {
//         .reset_gpio_num = PIN_RST,
//         .rgb_ele_order  = LCD_RGB_ELEMENT_ORDER_BGR,
//         .bits_per_pixel = 16,
//     };
// #if defined(USE_ST7789)
//     ESP_LOGI(TAG, "Driver: ST7789");
//     ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_cfg, &panel_handle));
// #elif defined(USE_ILI9341)
//     ESP_LOGI(TAG, "Driver: ILI9341");
//     ESP_ERROR_CHECK(esp_lcd_new_panel_ili9341(io_handle, &panel_cfg, &panel_handle));
// #else
//     #error "Define either USE_ST7789 or USE_ILI9341 above"
// #endif
//
//     // --------------------------------------------------------
//     // Reset, init, turn on
//     // --------------------------------------------------------
//     ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
//     ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
//     ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
//
//     // Mirror / swap if your image is flipped or rotated
//     // esp_lcd_panel_mirror(panel_handle, true, false);
//     // esp_lcd_panel_swap_xy(panel_handle, true);
//     // esp_lcd_panel_invert_color(panel_handle, true);
//
//     ESP_LOGI(TAG, "Panel ready — drawing test pattern");
//
//     // --------------------------------------------------------
//     // Test sequence:
//     //   1. Fill RED   → wait 1s
//     //   2. Fill GREEN → wait 1s
//     //   3. Fill BLUE  → wait 1s
//     //   4. RGB stripe pattern → loop
//     // --------------------------------------------------------
//     fill_color(panel_handle, 0xF800);   // RED
//     vTaskDelay(pdMS_TO_TICKS(1000));
//
//     fill_color(panel_handle, 0x07E0);   // GREEN
//     vTaskDelay(pdMS_TO_TICKS(1000));
//
//     fill_color(panel_handle, 0x001F);   // BLUE
//     vTaskDelay(pdMS_TO_TICKS(1000));
//
//     draw_test_pattern(panel_handle);
//
//     ESP_LOGI(TAG, "Test pattern displayed — check your screen!");
//     ESP_LOGI(TAG, "If colors look inverted, toggle rgb_endian between BGR and RGB");
//     ESP_LOGI(TAG, "If screen is blank, try switching driver (USE_ST7789 <-> USE_ILI9341)");
//
//     while (1) {
//         vTaskDelay(pdMS_TO_TICKS(5000));
//         ESP_LOGI(TAG, "Still running...");
//     }
// }
void app_main(void) {

    printf("Hello, World!\n");
    I2C_screen c;
    initI2C(&c);
    initLCD(&c);
    testProg(&c);

}

