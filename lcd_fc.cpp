
#include "pico/stdlib.h"

#include "pico/cyw43_arch.h"

#include "lcd_fc.h"
//#include "st7789.h"

#include <stdarg.h>
#include <stdio.h>
//#include <stdio>

char wireless_tty[CHAR_ROWS * CHAR_COLS];
char battery_tty[CHAR_ROWS * CHAR_COLS];
char ecu_tty[CHAR_ROWS * CHAR_COLS];



    // lcd configuration
    const struct st7789_config lcd_config_standalone = {
        .spi      = spi0,
        .gpio_din = 19,
        .gpio_clk = 18,
        .gpio_cs  = 21,
        .gpio_dc  = 17,
        .gpio_rst = 20,
        .gpio_bl  = 16,
    };
    

extern "C" void st7789_init(const struct st7789_config* config, uint16_t width, uint16_t height);
extern "C" void st7789_write(const void* data, size_t len);
extern "C" void st7789_put(uint16_t pixel);
extern "C" void st7789_fill(uint16_t pixel);
extern "C" void st7789_set_cursor(uint16_t x, uint16_t y);
extern "C" void st7789_vertical_scroll(uint16_t row);



LCD::LCD()
{
    
    lcd_init();
}

LCD::~LCD()
{

}


      void LCD::lcd_init(void)
      {
        //printf("Inicializace LCD");
            //initialize lcd
        st7789_init(&lcd_config, LCD_WIDTH, LCD_HEIGHT);
        // make screen black
        st7789_fill(0x0000);
        // make screen white
        //st7789_fill(0xffff);

        set_current_tty(battery_tty);
        //println(0,"Mode 240x320");

        //snprintf( &debug_tty[0*CHAR_COLS], CHAR_COLS, "Mode 240x320");
        //update_row(0);
        //println(debug_tty,0,"Mode 340x240");
        //println(0,"Mode 340x240");
        //printlnt(wireless_tty,0,"Wireless:");
        //printlnt(battery_tty,0,"Battery:");
        //printlnt(ecu_tty,0,"ECU:");
        //printlnt(debug_tty,0,"Debug:");
    
      }





char LCD::get_bit(uint8_t *array, uint32_t index) {
    return 1 & (array[index / 8] >> (index % 8));
}


void LCD::draw_scanline(const char *chars, uint32_t y) {
	static uint8_t scanbuf[LCD_WIDTH / 8];

	//fonts defined as simple array
	for (uint32_t i = 0; i < CHAR_COLS; ++i) {
		uint32_t c = chars[i + y / FONT_CHAR_HEIGHT * CHAR_COLS];
		scanbuf[i] =  SVGA141__8x16mirr[c*FONT_CHAR_HEIGHT +  y % FONT_CHAR_HEIGHT];
        //scanbuf[i] =  Font12_Table[c*FONT_CHAR_HEIGHT +  y % FONT_CHAR_HEIGHT];
	}

    //tady se zapisuje do displeje scanbuf jako lajna
        st7789_set_cursor(0, y);
    	for (uint32_t i = 0; i < LCD_WIDTH; ++i) {
            if(get_bit(scanbuf, i )) st7789_put(0xffff);
            else st7789_put(0x0000);
        }

}


void LCD::update_row(uint32_t row)
{
    for(uint32_t scanline = row * FONT_CHAR_HEIGHT; scanline < (row + 1) * FONT_CHAR_HEIGHT; scanline++)
    {
      draw_scanline(current_tty, scanline);
    }

}

void LCD::update_all(void)
{
 uint32_t row;
 for(row=0;row<CHAR_ROWS;row++)
 {
      update_row(row);
 }
}

void LCD::clear_all(void)
{
 memset(current_tty,0,CHAR_COLS*CHAR_ROWS);
 update_all();
}

/*
void scanline_callback() {
	static uint32_t y = 1;
	prepare_scanline(charbuf, y);
	y = (y + 1) % LCD_HEIGHT;
}
*/


void LCD::set_current_tty(char* tty)
{
    current_tty = tty;
    clear_all();
    update_all();
}

void LCD::printlnt(char* tty, const uint8_t row, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    memset(&tty[row*CHAR_COLS],0,CHAR_COLS);
    vsnprintf(&tty[row*CHAR_COLS],CHAR_COLS,fmt,args);
    va_end(args);
    if(tty == current_tty) update_row(row);
}

void LCD::println(const uint8_t row, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    memset(&current_tty[row*CHAR_COLS],0,CHAR_COLS);
    vsnprintf(&current_tty[row*CHAR_COLS],CHAR_COLS,fmt,args);
    va_end(args);
    update_row(row);
}

void LCD::act_tty_forward()
{
    //battery -> ecu -> wireless -> debug ->
    if (current_tty == battery_tty)
    {
       set_current_tty(ecu_tty); 
       return;
    }

    if (current_tty == ecu_tty)
    {
       set_current_tty(wireless_tty); 
       return;
    }

    if (current_tty == wireless_tty)
    {
       set_current_tty(debug_tty); 
       return;
    }

    if (current_tty == debug_tty)
    {
       set_current_tty(battery_tty); 
       return;
    }

    
}

void LCD::act_tty_backward()
{
        //reverese order(battery -> ecu -> wireless -> debug ->)
    if (current_tty == battery_tty)
    {
       set_current_tty(debug_tty); 
       return;
    }

    if (current_tty == ecu_tty)
    {
       set_current_tty(battery_tty); 
       return;
    }

    if (current_tty == wireless_tty)
    {
       set_current_tty(ecu_tty); 
       return;
    }

    if (current_tty == debug_tty)
    {
       set_current_tty(wireless_tty); 
       return;
    }


}


