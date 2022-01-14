/*
 * USB bulk transfer example with UART communication
 * 
 * Copyright (c) 2022 Valerio Spinogatti
 * Licensed under GNU license
 * 
 * uart.h
 */

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>


#ifndef UART_H
#define UART_H

/*Initialize UART*/
void uart_init(void);


/*Send buffer over UART by inserting a linefeed every time
that the index is a multiple of 64*/
void uart_tx(uint8_t *buffer, uint16_t len);

#endif