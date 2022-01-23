/*
 * UART to USB converter
 * 
 * Copyright (c) 2022 Valerio Spinogatti
 * Licensed under GNU license
 * 
 * util.h
 */

#ifndef UTIL_H
#define UTIL_H


#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include "common.h"


/**
 * @brief Blink the LED connected to PC13
 */
void led_blink(void);

/**
 * @brief Initialize system
 */
void sys_init(void);

#endif