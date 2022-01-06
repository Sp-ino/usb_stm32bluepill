/*
 * USB Tutorial
 * 
 * Copyright (c) 2020 Manuel Bleichenbacher
 * Licensed under MIT License
 * https://opensource.org/licenses/MIT
 * 
 * USB descriptor
 */

#ifndef USB_DESCRIPTOR_H
#define USB_DESCRIPTOR_H

#include <libopencm3/usb/usbd.h>

#define MAX_PACKET_SIZE 16

// Interface index
#define INTF_COMM 0

#define LED_CONTROL_ID 0x33
#define PUSHBUTTON_REQ_ID 0x34

// USB descriptor string table
extern const char *const usb_desc_strings[4];
// USB device descriptor
extern const struct usb_device_descriptor usb_device_desc;
// USB device configurations
extern const struct usb_config_descriptor usb_config_descs[];

void usb_init_serial_num();

#endif
