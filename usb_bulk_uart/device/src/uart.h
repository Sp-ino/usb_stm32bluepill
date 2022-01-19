/*
 * USB bulk transfer example with UART communication
 * 
 * Copyright (c) 2022 Valerio Spinogatti
 * Licensed under GNU license
 * 
 * uart.h
 */

#ifndef UART_H
#define UART_H


/**
 * @brief Initialize UART
 */
void uart_init(void);

/**
 * @brief Send buffer over UART by inserting a linefeed every time
 * that the index is a multiple of 64
 * 
 * @param buffer 
 * @param len 
*/
void uart_tx(uint8_t *buffer, uint16_t len);

#endif