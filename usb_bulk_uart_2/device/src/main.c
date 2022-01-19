/*
 * USB bulk transfer example with UART communication - 2
 * 
 * Copyright (c) 2022 Valerio Spinogatti
 * Licensed under GNU license
 * 
 * main.c
 */

#include "usbcustom.h"
#include "util.h"
#include "uart.h"


/*main function*/
int main(void) 
{
    sys_init();
    usb_init();
    uart_init();

    //main loop
    while(1) 
    {
        uart_rx(data_buffer, data_size);
        handle_buffer_full();
    }

    return 0;
}