#include "pico/stdlib.h"
#include <cstdio>
#include "buttons.h"

#define MAXIMUM 10*512
#define THRESHOLD 10*256

KEYPAD::KEYPAD()
{
  keylock = false;
}

BUTTONS::BUTTONS(uint GPIO)
{
  GPIO_id = GPIO;
  printf("Button %d init\n", GPIO_id);
    button_state=MAXIMUM;
    flag_rising = 0;
    flag_falling = 0;
    gpio_init(GPIO_id);
	  gpio_set_dir(GPIO_id, GPIO_IN);
	  gpio_pull_up(GPIO_id);
    button_prev_state = get();
}

void BUTTONS::update(void)
{
    
    button_new_state = get();

    if(button_new_state > button_prev_state) flag_rising = 1;

    if(button_new_state < button_prev_state) flag_falling = 1;
  

    button_prev_state = button_new_state;

    

    if(gpio_get(GPIO_id))
    {
        if(button_state < MAXIMUM) button_state++;
    }
    else
    {
        if(button_state > 0) button_state--;
    }

}

bool BUTTONS::get_direct()
{
    return gpio_get(GPIO_id);
}

uint8_t BUTTONS::get()
{
    if(button_state > THRESHOLD) return 1;
    else return 0;
}

uint8_t BUTTONS::rising_edge()
{
  if(flag_rising)
  {
    flag_rising = 0;
    return 1;
  }
  else return 0;
}

uint8_t BUTTONS::falling_edge()
{
  if(flag_falling)
  {
    flag_falling = 0;
    return 1;
  }
  else return 0;
}


