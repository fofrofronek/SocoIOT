#include "pico/stdlib.h"
#include <hardware/dma.h>
#include <hardware/gpio.h>
#include <hardware/irq.h>
#include <hardware/uart.h>
#include <string.h>



#include <stdio.h>


#include <pico/time.h>

//#include <pico/stdio_uart.h>

#include <hardware/dma.h>
#include <hardware/uart.h>

#define UART_ID uart0
#define BAUD_RATE 9600
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE

#define UART_TX_PIN 0
#define UART_RX_PIN 1

#include <uart_dma.h>

// The buffer size needs to be a power of two and alignment must be the same
//__attribute__((aligned(32)))
static char buffer[UART_BUFFER_SIZE];
volatile char publish_buffer[UART_BUFFER_SIZE];



const uint32_t channel = 1;
dma_channel_config config;
volatile uint8_t new_data = 0;


void dma_handler() {

    if(dma_channel_get_irq1_status(channel))
    {

    
     //printf("buffer: %.10s\n",buffer);
     memcpy((void*)publish_buffer,buffer,UART_BUFFER_SIZE);
     memset(buffer, 0, UART_BUFFER_SIZE);
     new_data = 1;
    
     //interrupt flag clear.
     dma_hw->ints1 = 1u << channel;
    
     // Give the channel address to re-trigger it
     dma_channel_set_write_addr(channel, buffer, true);
    
     printf("DMA tci at count %d\n", dma_channel_hw_addr(channel)->transfer_count);
    }
}


static void configure_dma(int channel) {
    config = dma_channel_get_default_config(channel);
    channel_config_set_transfer_data_size(&config, DMA_SIZE_8);

    // The read address is the address of the UART data register which is constant
    channel_config_set_read_increment(&config, false);

    // Write into a buffer
    channel_config_set_write_increment(&config, true);
   

    // The UART signals when data is avaliable
    channel_config_set_dreq(&config, DREQ_UART0_RX);

    // set transfer params
    dma_channel_configure(
        channel,
        &config,
        buffer,
        &uart0_hw->dr,
        UART_BUFFER_SIZE,
        false);

  // Tell the DMA to raise IRQ line 0 when the channel finishes a block
    dma_channel_set_irq1_enabled(channel, true);

    // Configure the processor to run dma_handler() when DMA IRQ 0 is asserted
    irq_set_exclusive_handler(DMA_IRQ_1, dma_handler);
    irq_set_enabled(DMA_IRQ_1, true);
    
    //trigger handler to start first transfer
    dma_handler();
}

static void configure_uart() {
    // The SDK seems to configure sane values for baudrate, etc.
    //stdio_uart_init();

        // Set up our UART with a basic baud rate.
    uart_init(UART_ID, 2400);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // Actually, we want a different speed
    // The call will return the actual baud rate selected, which will be as close as
    // possible to that requested
    //int __unused actual = uart_set_baudrate(UART_ID, BAUD_RATE);
    int actual = uart_set_baudrate(UART_ID, BAUD_RATE);
 
    // Set UART flow control CTS/RTS, we don't want these, so turn them off
    uart_set_hw_flow(UART_ID, false, false);

    // Set our data format
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);

    // Turn off FIFO's - we want to do this character by character
    //uart_set_fifo_enabled(UART_ID, false);
    uart_set_fifo_enabled(UART_ID, true);

    //We use DMA instead of byte-by byte
    // Set up a RX interrupt
    // We need to set up the handler first
    // Select correct interrupt for the UART we are using
    //int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;

    // And set up and enable the interrupt handlers
    //irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
    //irq_set_enabled(UART_IRQ, true);

    // Now enable the UART to send interrupts - RX only
    //uart_set_irq_enables(UART_ID, true, false);

    // OK, all set up.
    // Lets send a basic string out, and then run a loop and wait for RX interrupts
    // The handler will count them, but also reflect the incoming data back with a slight change!
    //uart_puts(UART_ID, "\nHello, uart interrupts\n");


    // On my system there is one junk byte on boot
    //uart_getc(uart0);
}

void uart_dma_start(void)
{
    memset(buffer, 0 , UART_BUFFER_SIZE);
    memset((void*)publish_buffer, 0 , UART_BUFFER_SIZE);
    
    configure_uart();
    configure_dma(channel);
    
}

void uart_poll_config(void)
{
    memset(buffer, 0 , UART_BUFFER_SIZE);
    memset((void*)publish_buffer, 0 , UART_BUFFER_SIZE);
    
    configure_uart();
    //configure_dma(channel);
    
}


int main_serial() {
    

    configure_uart();
    configure_dma(channel);

    memset(buffer, 0 , UART_BUFFER_SIZE);

    for (;;) {
        // Print out the contents of the buffer
        //printf("buffer: '%.*s' transfer_count=%u\n",
        //        (int)sizeof(buffer), buffer,
        //        dma_channel_hw_addr(channel)->transfer_count);

        tight_loop_contents();

        //sleep_ms(1000);
    }
}