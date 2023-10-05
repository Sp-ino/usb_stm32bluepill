/*
 * UART to USB converter
 * 
 * Copyright (c) 2022 Valerio Spinogatti
 * Licensed under GNU license
 * 
 * userint.h
 */

#ifndef USBCUSTOM_H
#define USBCUSTOM_H


#include <libopencm3/usb/usbd.h>
#include "descriptor.h"

//macro definitions
#define BUFFER_LEN_PACKETS 2
#define BUFFER_LEN_BYTES ( BUFFER_LEN_PACKETS * BULK_MAX_PACKET_SIZE )

/*-------------------------------Function prototypes------------------------------*/

/**
* @brief Initialize usb.
*/
void serial_interf_init(void);


/**
 * @brief This callback is executed when the host sets the device configuration.
 * 
 * @param usbd_dev 
 * @param wValue 
 */
void usb_set_config(usbd_device *usbd_dev, 
                    uint16_t wValue __attribute__((unused)));


/**
 * @brief This function is called from the while loop inside the main. It tries
 * to keep the buffer empty by resending bytes over UART.
 */
void handle_main_tasks(void);

#endif