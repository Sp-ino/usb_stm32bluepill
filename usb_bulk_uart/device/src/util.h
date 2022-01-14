#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include "common.h"


#ifndef UTIL_H
#define UTIL_H

/*Blink the LED connected to PC13*/
void led_blink(void);

/*Initialize system*/
void sys_init(void);

#endif