//
// Created by pat on 21/05/2026.
//

#include "screen.h"
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "driver/i2c_master.h"
#include "esp_lcd_io_i2c.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_sh1106.h"

#define I2C_HOST I2C_NUM_0



void show() {

}
void hide() {

}
void initI2C(I2C_screen *s){
    // i2c bus configuration
    s -> bus_config = (i2c_master_bus_config_t){
        .i2c_port = I2C_HOST,               // I2C port number
        .sda_io_num = (gpio_num_t)8,  // GPIO number for I2C sda signal, set from "idf menuconfig"
        .scl_io_num = (gpio_num_t)9,  // GPIO number for I2C scl signal, set from "idf menuconfig"
        .clk_source = I2C_CLK_SRC_DEFAULT,  // I2C clock source, just use the default
        .glitch_ignore_cnt = 7,             // glitch filter, again, just use the default
        .intr_priority = 0,                 // interrupt priority, default to 0
        .trans_queue_depth = 0,             // transaction queue depth, default to 0
        .flags = {
            .enable_internal_pullup = true, // enable internal pullup resistors (oled screen does not have one)
            .allow_pd = false,              // just using the default value
        },
    };

    // Create the i2c bus handle
    s->i2c_bus_handle = NULL;
    ESP_ERROR_CHECK(i2c_new_master_bus(&s->bus_config, &s->i2c_bus_handle));

    // Create the i2c io handle
    s->io_handle = NULL;
    s->io_config = (esp_lcd_panel_io_i2c_config_t)ESP_SH1106_DEFAULT_IO_CONFIG;
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(s->i2c_bus_handle, &s->io_config, &s->io_handle));
}
void initLCD(I2C_screen *s){
    // sh1106 panel configuration (most of the values are not used, but must be set to avoid cpp warnings)
    s->panel_config = (esp_lcd_panel_dev_config_t){
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,   // not even used, but must be set to avoid cpp warnings
        .data_endian = LCD_RGB_DATA_ENDIAN_LITTLE,    // not even used, but must be set to avoid cpp warnings
        .bits_per_pixel = SH1106_PIXELS_PER_BYTE / 8, // bpp = 1 (monochrome, that's important)
        .reset_gpio_num = (gpio_num_t)-1,                         // sh1106 does not have a reset pin, so set to -1
        .vendor_config = NULL,                        // no need for custom vendor config, not implemented
        .flags = {
            .reset_active_high = false,               // not even used, but must be set to avoid cpp warnings
        },
    };

    // Create the panel handle from the sh1106 driver
    s->panel_handle = NULL;
    ESP_ERROR_CHECK(esp_lcd_new_panel_sh1106(s->io_handle, &s->panel_config, &s->panel_handle));
    // Reset the screen
    ESP_ERROR_CHECK(esp_lcd_panel_reset(s->panel_handle));
    // Initialize the screen
    ESP_ERROR_CHECK(esp_lcd_panel_init(s->panel_handle));
    // Turn on the screen
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(s->panel_handle, true));

    //memory space of the buffer of the screen
    memset(s->lcd_panel_buffer, 0, SH1106_BUFFER_SIZE);
}
void clear() {

}
void drawStr(char *s) {

}
void drawLine(int x1, int y1, int x2, int y2,I2C_screen s) {
    int a = (x1 - x2);
    if (a == 0) {
        a=x1;
    }else {
        a = (y1 - y2) / a;
        int b = y1 - (x1 * a);
        for (int i = x1; i < x2 + 1; ++i) {
            s.lcd_panel_buffer[a * i + b] |= (1ULL << i);
        }
    }
}


void clearRect(int x, int y, int w, int h) {

}
void drawRect(int x, int y, int w, int h) {

}
void drawImage(int x, int y, int w, int h) {

}
void testProg(I2C_screen *s) {
    s->lcd_panel_buffer[0] = 0b11111111;
    s->lcd_panel_buffer[SH1106_BUFFER_SIZE - 1] = 0b11111111;
    // Send the buffer to the screen
    ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(s->panel_handle, 0, 0, SH1106_WIDTH, SH1106_HEIGHT, s->lcd_panel_buffer));
    vTaskDelay(pdMS_TO_TICKS(2000)); // wait a bit
}
