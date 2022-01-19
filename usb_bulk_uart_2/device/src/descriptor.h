/*
 * USB bulk transfer example with UART communication
 * 
 * Copyright (c) 2022 Valerio Spinogatti
 * Licensed under GNU license
 * 
 * usb_descriptor.h
 */

#ifndef USB_DESCRIPTOR_H
#define USB_DESCRIPTOR_H


#include <libopencm3/usb/usbd.h>


#define BULK_MAX_PACKET_SIZE 64

#define USB_VENDOR_ID 0x0297        //Vendor ID
#define USB_PRODUCT_ID 0x0297       //Product ID
#define USB_DEVICE_RELEASE 0x0100   //release 1.0.0
#define INTERF_NUM 0                //Interface index
#define EP_DATA_OUT 0x01            //Endpoint number for host-to-device data transmission
#define EP_DATA_IN 0x82

//USB descriptor strings
extern const char *const usb_strings[4];

//USB device descriptor
extern const struct usb_device_descriptor usb_device_desc;

//USB device configuration descriptor
extern const struct usb_config_descriptor usb_config_descs[];

#endif
