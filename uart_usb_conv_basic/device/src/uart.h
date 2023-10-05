/*
 * UART to USB converter
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
 * @brief Get a byte from UART1
 * 
 * @return uint8_t 
 */
uint8_t uart_getc(void);

#endif