/*
 * USB bulk transfer example with UART communication - 2
 * 
 * Copyright (c) 2022 Valerio Spinogatti
 * Licensed under GNU license
 * 
 * uart.c
 */

#include "uart.h"


void uart_init(void)
{
    rcc_periph_clock_enable(RCC_USART1);

    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_RX);

    usart_set_baudrate(USART1, 38400);
    usart_set_databits(USART1, 8);
    usart_set_stopbits(USART1, USART_STOPBITS_1);
    usart_set_mode(USART1, USART_MODE_RX);
    usart_set_parity(USART1, USART_PARITY_NONE);
    usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
    usart_enable(USART1);
}


void uart_rx(uint8_t *buffer, uint16_t datasize)
{
    uint8_t byte;
    byte = usart_rcv(USART1);
    buffer[datasize] = byte

    gpio_toggle(GPIOC, GPIO13);   

    if (datasize > BUFFER_MAX_DATA_SIZE)
    {
        is_buffer_full = true;
    }
}