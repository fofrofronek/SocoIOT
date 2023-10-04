#include "pico/stdlib.h"
extern volatile uint8_t core1_wd_flag;
void main_ble_core1()
{
    while(true)
    {
        core1_wd_flag = 0xFF;
        tight_loop_contents();
    }
}