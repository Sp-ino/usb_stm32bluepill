/*
 * USB bulk transfer example with UART communication
 * 
 * Copyright (c) 2022 Valerio Spinogatti
 * Licensed under GNU license
 * 
 * uart.h
 */

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/usb/usbd.h>
#include <stdlib.h>
#include "usb_descriptor.h"
#include "common.h"
#include "util.h"

#ifndef USBCUSTOM_H
#define USBCUSTOM_H

//constant definitions
#define BUFFER_LEN_PACKETS 4
#define BUFFER_LEN_BYTES ( BUFFER_LEN_PACKETS * BULK_MAX_PACKET_SIZE )
#define BUFFER_MAX_OCCUPIED_SIZE ( BUFFER_LEN_BYTES - 2*BULK_MAX_PACKET_SIZE )
#define BUFFER_OKREAD_SIZE 0
#define TX_BUFFER_LEN_BYTES 128


/*-------------------------------Function prototypes------------------------------*/

/*Initialize usb*/
void usb_init(void);

/*This callback is executed during USB configuration*/
void usb_set_config(usbd_device *usbd_dev, uint16_t wValue __attribute__((unused)));

/*This callback function is executed when a packet is received. 
It stores the packet inside the buffer and sets the endpoint to 
NAK when the buffer is full*/
void handle_bulk_rx_cb(usbd_device *usbd_dev, uint8_t ep __attribute__((unused)));

/*This function takes care of sending characters over the UART by
calling the uart_tx function when the buffer is full. Once data has 
been sent, the buffer is shifted towards left by the exact amount 
of bytes that has been transmitted and the OUT endpoint is set to 
VALID, so that new data can be received*/
void handle_bulk_tx(void);

/*USB interrupt handler; used to call usbd_poll()
(must be called periodically for the USB communication 
to work properly)*/
void handle_buffer_full(void);

#endif