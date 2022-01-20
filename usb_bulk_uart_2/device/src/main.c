/*
 * USB bulk transfer example with UART communication
 * 
 * Copyright (c) 2022 Valerio Spinogatti
 * Licensed under GNU license
 * 
 * main.c
 */

#include "serialinterf.h"
#include "util.h"


/*main function*/
int main(void) 
{
    sys_init();
    serial_interf_init();

    //main loop
    while(1) 
    {
        handle_main_tasks();
    }

    return 0;
}