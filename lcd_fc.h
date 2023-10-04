#pragma once

#ifndef _LCD_FC_H_
#define _LCD_FC_H_

#include "font8x16.h"

#define LCD_WIDTH  240
#define LCD_HEIGHT  320

#define CHAR_COLS (LCD_WIDTH / FONT_CHAR_WIDTH)
#define CHAR_ROWS (LCD_HEIGHT / FONT_CHAR_HEIGHT)

extern char wireless_tty[CHAR_ROWS * CHAR_COLS];
extern char battery_tty[CHAR_ROWS * CHAR_COLS];
extern char ecu_tty[CHAR_ROWS * CHAR_COLS];


#include "pico/stdlib.h"
#include "hardware/spi.h"

struct st7789_config {
    spi_inst_t* spi;
    uint32_t gpio_din;
    uint32_t gpio_clk;
    uint32_t gpio_cs;
    uint32_t gpio_dc;
    uint32_t gpio_rst;
    uint32_t gpio_bl;
};

class LCD{
    private:
    // lcd configuration
    const struct st7789_config lcd_config = {
        .spi      = spi0,
        .gpio_din = 19,
        .gpio_clk = 18,
        .gpio_cs  = 21,
        .gpio_dc  = 17,
        .gpio_rst = 20,
        .gpio_bl  = 16,
    };
    
    char* current_tty; //tty is being drawn on the lcd
    char get_bit(uint8_t *array, uint32_t index);
    void draw_scanline(const char *chars, uint32_t y);
    void update_row(uint32_t row);
    void update_all(void);
    

    public:
    char debug_tty[CHAR_ROWS * CHAR_COLS];
    LCD();
    ~LCD();
    void lcd_init(void);
    void printlnt(char* tty, const uint8_t row, const char *fmt, ...);
    void println(const uint8_t row, const char *fmt, ...);
    void set_current_tty(char* tty);
    void clear_all(void);
    void act_tty_forward();
    void act_tty_backward();
};

#endif
