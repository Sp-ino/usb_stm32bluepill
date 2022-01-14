#include "usbcustom.h"
#include "util.h"
#include "uart.h"


/*main loop*/
int main(void) 
{
    sys_init();
    usb_init();
    uart_init();

    while(1) 
    {
        handle_buffer_full();
    }

    return 0;
}