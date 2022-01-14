#include "util.h"


void led_blink(void) 
{
    for (int i = 0; i < 8; i++) 
    {
        gpio_toggle(GPIOC, GPIO13);
        delay(50);
    }
}


void sys_init(void) 
{
    rcc_clock_setup_in_hse_8mhz_out_72mhz();
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOC);

    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
    gpio_set(GPIOC, GPIO13);

    systick_init();
}