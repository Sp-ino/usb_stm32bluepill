# usb_stm32bluepill
## Code written while learning USB communication on stm32 bluepill.

my_usb_blink contains a modified version of the code from the usb-tutorial repository.

usb_bulk contains code that exemplifies usb bulk transfer in both directions (IN and OUT).

usb_bulk_uart contains sw and fw that allows the stm32 to read bytes from USB and to send them back over UART. Bytes are stored inside of a buffer and sent over UART when the buffer is full.

usb_bulk_uart_2 contains sw and fw that allows the stm32 to read bytes from USB and to send them back over UART. This project, however, uses a different approach to sendimmediately over UART everything that is received (without waiting for the buffer to be full).
