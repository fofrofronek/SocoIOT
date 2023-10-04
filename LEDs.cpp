#include "pico/stdlib.h"
#include <cstdio>
#include "LEDs.h"
#define USR 22
#define LCD_BL 16

LEDs::LEDs()
{


}
void LEDs::Init()
{
    printf("LEDs init\n");
    gpio_init(USR);
    gpio_put(USR, 1);
	gpio_set_dir(USR, GPIO_OUT);

    gpio_init(LCD_BL);
    gpio_put(LCD_BL, 1);
	gpio_set_dir(LCD_BL, GPIO_OUT);
}

void LEDs::USRled_on()
{
    gpio_put(USR, 0);
}

void LEDs::USRled_off()
{
    gpio_put(USR, 1);   
}

void LEDs::LCD_blank()
{
  gpio_put(LCD_BL, 0);  
}

void LEDs::LCD_on()
{
  gpio_put(LCD_BL, 1);  
}

bool LEDs::LCD_get()
{
    return gpio_get(LCD_BL);
}