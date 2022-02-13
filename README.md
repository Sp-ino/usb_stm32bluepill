# usb_stm32bluepill
## Code written while learning USB communication on stm32 bluepill.

* __my_usb_blink__ contains a modified version of the code from the usb-tutorial repository.

* __usb_bulk__ contains code that exemplifies usb bulk transfer in both directions (IN and OUT).

* __usb_bulk_uart__ contains sw and fw that allows the stm32 to read bytes from USB and to send them back over UART. Bytes are stored inside of a buffer and sent over UART when the buffer is full.

* __usb_bulk_uart_2__ contains sw and fw that allows the stm32 to read bytes from USB and to send them back over UART. This project, however, uses a different approach to sendimmediately over UART everything that is received (without waiting for the buffer to be full).


### Notes
- Host python applications for Windows are signified with a '\_win' appended to the end of their names.
- The bluepill USB device needs libusbK as USB driver to work on windows. This driver can be installed with Zadig.
