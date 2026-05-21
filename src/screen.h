//
// Created by pat on 21/05/2026.
//





#include <string.h>
#include "freertos/FreeRTOS.h"
#include "driver/i2c_master.h"
#include "esp_lcd_io_i2c.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_sh1106.h"

#define I2C_HOST I2C_NUM_0






    // // turn all the pixels on
    // memset(buffer_data, 0xFF, SH1106_BUFFER_SIZE);
    // // only update a portion of the screen to show that partial updates work
    // ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(panel_handle, 48, 16, 80, 48, buffer_data));
    // vTaskDelay(pdMS_TO_TICKS(2000)); // wait a bit
    //
    //
    // // turn all the pixels white using a sliding window and small buffer
    // memset(buffer_data, 0x00, SH1106_BUFFER_SIZE);
    // memset(buffer_data, 0xFF, 8); // only turn on 8x8 pixels at a time to show that small updates work
    // for (int y = 0; y < SH1106_HEIGHT; y += 8) {
    //     for (int x = 0; x < SH1106_WIDTH; x += 8) {
    //         ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(panel_handle, x, y, x + 8, y + 8, buffer_data));
    //         vTaskDelay(pdMS_TO_TICKS(50)); // add a small delay to make the sliding window effect visible
    //     }
    // }

#ifndef C3_SUPERMINI_ESPIDF_SCREEN_H
#define C3_SUPERMINI_ESPIDF_SCREEN_H



typedef struct {
    // I2C CONFIG //
    i2c_master_bus_config_t bus_config;
    i2c_master_bus_handle_t i2c_bus_handle;
    esp_lcd_panel_io_handle_t io_handle ;
    esp_lcd_panel_io_i2c_config_t io_config ;
    // LCD CONFIG //
    esp_lcd_panel_dev_config_t panel_config;
    esp_lcd_panel_handle_t panel_handle;
    uint8_t lcd_panel_buffer[SH1106_BUFFER_SIZE];
}I2C_screen;


void initI2C(I2C_screen *s);
void initLCD(I2C_screen *s);
void testProg(I2C_screen *s);

#endif //C3_SUPERMINI_ESPIDF_SCREEN_H