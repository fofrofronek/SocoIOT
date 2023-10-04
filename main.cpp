#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/multicore.h"
#include "hardware/watchdog.h"
//#include "lwip/timeouts.h"
#include "lcd_fc.h"
#include "uart_dma.h"
#include "SuperSocoParser.h"
#include "buttons.h"
#include "LEDs.h"
#include "lwip_iot.h"
#include "SdConfData.h"



extern "C" void uart_dma_start(void);
extern "C" void uart_poll_config(void);

void main_ble_core1();

volatile uint8_t core1_wd_flag = 0;


pktStat pktStat485;
SD Card;
LEDs Leds;

void core1_prelaunch()
{
    //this should be called acording some sources
    //but makes the lwip not working on core 1
    //sys_timeouts_init();
}

void core1_entry()
{
    main_iot_core1();
}

void core1_postlaunch()
{

}



void update_screen(LCD* screen)
{
 switch(publish.Breaker){
    case 0:
        screen->printlnt(battery_tty,0,"Battery: OK");
    break;
    case 1:
        screen->printlnt(battery_tty,0,"Battery: BMS stopped charge");
    break;
    case 2:
        screen->printlnt(battery_tty,0,"Battery: Too high charge current");
    break;
    case 4:
        screen->printlnt(battery_tty,0,"Battery: Too high discharge current");
    break;
    default:
        screen->printlnt(battery_tty,0,"Battery: Undecoded status");
    break;

 }

 screen->printlnt(battery_tty,2,"Voltage:  %d V",publish.Voltage);
 screen->printlnt(battery_tty,4,"SOC:      %d %%",publish.SOC);
 screen->printlnt(battery_tty,6,"Cycles:   %d / %d -",publish.Cycle, publish.Fullcycle);
 screen->printlnt(battery_tty,8,"BMS temp: %d %cC",publish.BMStemp,(char)0xF8);
 screen->printlnt(battery_tty,10,"Charging: %d -",publish.Charging);
 screen->printlnt(battery_tty,12,"Current:  %d A", abs(publish.Bcurrent));
 screen->printlnt(battery_tty,19,"Rcvd:      %d", pktStat485.rcvd_chunks);

 screen->printlnt(ecu_tty,0,"ECU:");
 screen->printlnt(ecu_tty,2,"Speed:        %d km/h",publish.Speed);
 
 screen->printlnt(ecu_tty,5,"Power:        %d W", (publish.Ecurrent * publish.Voltage)/10);
 
 screen->printlnt(ecu_tty,7,"Current:      %d A",publish.Ecurrent/10);
 
 
 screen->printlnt(ecu_tty,10,"ECU temp:     %d %cC",publish.ECUtemp,(char)0xF8);
 
 screen->printlnt(ecu_tty,12,"Riding mode:  %d",publish.Mode);
 screen->printlnt(ecu_tty,19,"Rcvd: %d",pktStat485.rcvd_chunks );
 
 screen->printlnt(wireless_tty,0,"Wireless:");
 screen->printlnt(wireless_tty,1,"Adapter:     %s", LWIPstatus.hw_initialized ? "OK" : "Fail");
 screen->printlnt(wireless_tty,2,"AP1 conn:    %s", LWIPstatus.AP1_ok ? "Yes" : "No");
 screen->printlnt(wireless_tty,3,"AP2 conn:    %s", LWIPstatus.AP2_ok ? "Yes" : "No");
 screen->printlnt(wireless_tty,4,"Client init: %s", LWIPstatus.init_client_instance ? "OK" : "Fail");
 screen->printlnt(wireless_tty,5,"Broker init: %s", LWIPstatus.init_broker_connection ? "OK" : "Fail");
 screen->printlnt(wireless_tty,6,"Conn errors: %d", LWIPstatus.conn_timeouts);
 screen->printlnt(wireless_tty,7,"Publish err: %d", LWIPstatus.publish_errors);

 screen->printlnt(wireless_tty,19,"Rcvd: %d",pktStat485.rcvd_chunks);
 
 screen->printlnt(screen->debug_tty,0,"Debug:");
 screen->printlnt(screen->debug_tty,2,"Version: 0.1.0");
 screen->printlnt(screen->debug_tty,4,"Rcvd total: %d",pktStat485.rcvd_total);
 screen->printlnt(screen->debug_tty,6,"Rcvd BMS: %d",pktStat485.rcvd_BMS );
 screen->printlnt(screen->debug_tty,8,"Rcvd ECU: %d",pktStat485.rcvd_ECU );
 screen->printlnt(screen->debug_tty,10,"Rcvd GSM: %d",pktStat485.rcvd_GSMGPS );
 screen->printlnt(screen->debug_tty,12,"Rcvd UNK: %d",pktStat485.rcvd_UNKNOWN );
 screen->printlnt(screen->debug_tty,19,"Rcvd:  %d",pktStat485.rcvd_chunks);

}



int main() //iot on core1
{
    
    stdio_init_all();
   
    //busy_wait_ms(6000);
    if (watchdog_caused_reboot()) {
        busy_wait_ms(6000);
        printf("Rebooted by Watchdog!\n");
        //return 0;
    } else {
        printf("Regular boot\n");
    }

    //watchdog set to maximum approx. 8s  
    watchdog_enable(0x7FFFFF, 1);

    Leds.Init();

    KEYPAD kp;

    BUTTONS button_a(BUTTON_A);
    BUTTONS button_x(BUTTON_X);
    BUTTONS button_dwn(BUTTON_DOWN);
    BUTTONS button_rgt(BUTTON_RIGHT);
    BUTTONS button_up(BUTTON_UP);

    Card.Init();
    Config ConfigFile;

    LCD screen;
    
    std::vector<unsigned char> SerialChunk;

    
    //launch lwip
    
    if(button_rgt.get_direct()==1)
    {
     core1_prelaunch();
     multicore_launch_core1(main_iot_core1);
     core1_postlaunch();
    }
    else //run BLE in the future
    {
     core1_prelaunch();
     multicore_launch_core1(main_ble_core1);
     core1_postlaunch();
    }

    //uart_dma_start(); uart dma doesn't work with lwip for some reason
    uart_poll_config();
    uint32_t charcnt=0;
    absolute_time_t timeout = nil_time;
    
    
    while(1) {
        if(core1_wd_flag) 
        {   
            core1_wd_flag = 0x00;
            watchdog_update();
        }

        button_a.update();
        button_rgt.update();
    
        if(!kp.keylock)
        {
            button_x.update();
            button_dwn.update();
            button_up.update();
            
            if(button_up.falling_edge()) screen.act_tty_forward();
            if(button_dwn.falling_edge()) screen.act_tty_backward();
            if(button_x.falling_edge()) 
            {
                if(Card.is_logging()) Card.StopLogging();
                else Card.StartLogging();
            }

            if(button_a.falling_edge()) 
            {
              kp.keylock = true;
              Leds.LCD_blank();
            }            
        }
        else
        {
            if(button_a.falling_edge() && button_rgt.get()==0)
            {
                kp.keylock = false;
                Leds.LCD_on();
            }
        }
    
        //if(Card.is_logging()) {Card.DoLogging((char*)publish_buffer,256); Card.StopLogging();}

        if(uart_is_readable(uart0))
        {
            publish_buffer[charcnt] = uart_getc(uart0);
            charcnt++;
            if(charcnt >= UART_BUFFER_SIZE)
            {
                printf("Uart poll read %d bytes\n", charcnt);
                Card.DoLogging((char*)publish_buffer,charcnt);
                charcnt = 0;
                new_data = 1;
            }
        }


        if(new_data)
        {
        
            printf("new_data == 1\n");       
            //printf("publish_buffer: %.10s\n",publish_buffer);
            SerialChunk.assign(std::begin(publish_buffer), std::end(publish_buffer));
            new_data=0;
            ParseChunk(SerialChunk);

            
            pktStat485.rcvd_chunks++;

        }

        absolute_time_t now = get_absolute_time();
            if (is_nil_time(timeout) || absolute_time_diff_us(now, timeout) <= 0) {
            
            update_screen(&screen);

            timeout = make_timeout_time_ms(40);

        }

        //tight_loop_contents();
    }

}
