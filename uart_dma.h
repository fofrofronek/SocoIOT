
#pragma once
#ifndef _UART_DMA_H_
#define _UART_DMA_H_

#include <hardware/uart.h>

#define UART_BUFFER_SIZE 256
//buffer containing data
extern volatile char publish_buffer[UART_BUFFER_SIZE];
//flag gets 1 if there are new data in publish_buffer
extern volatile uint8_t new_data;

//extern "C" void uart_dma_start(void);

#endif