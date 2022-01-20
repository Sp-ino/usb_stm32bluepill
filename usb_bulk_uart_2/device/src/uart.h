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
 * @brief Init UART
 */
void uart_init(void);

/**
 * @brief Send a byte over UART1
 * 
 * @param buffer 
 * @param len 
*/
void uart_tx(uint8_t c);

#endif