/*
 * USB bulk transfer example with UART communication - 2
 * 
 * Copyright (c) 2022 Valerio Spinogatti
 * Licensed under GNU license
 * 
 * usb_descriptor.c
 */

#include "descriptor.h"
#include <stdlib.h>


const char *const usb_strings[] = 
{
    "spino",                //USB Manufacturer
    "Repeater",             //USB Product
    "VLSP01",               //Serial number
    "Repeater Interface",   //Data interface
};


enum usb_strings_index
{
    STRINGS_MANUFACTURER_ID = 1,
    STRINGS_PRODUCT_ID,
    STRINGS_SERIAL_NUMBER_ID,
    STRINGS_DATA_INTERF_ID,
};


static const struct usb_endpoint_descriptor comm_endpoint_descs[] = 
{
    {
        .bLength = USB_DT_ENDPOINT_SIZE,
        .bDescriptorType = USB_DT_ENDPOINT,
        .bEndpointAddress = EP_DATA_IN,                //endpoint for host-to-device transmission
        .bmAttributes = USB_ENDPOINT_ATTR_BULK,
        .wMaxPacketSize = BULK_MAX_PACKET_SIZE,
        .bInterval = 0,
        .extra = NULL,
        .extralen = 0,
    },
};


static const struct usb_interface_descriptor comm_interf_descs[] = 
{
    {
        .bLength = USB_DT_INTERFACE_SIZE,
        .bDescriptorType = USB_DT_INTERFACE,
        .bInterfaceNumber = INTERF_NUM,
        .bAlternateSetting = 0,
        .bNumEndpoints = sizeof(comm_endpoint_descs) / sizeof(comm_endpoint_descs[0]),
        .bInterfaceClass = USB_CLASS_VENDOR,            //vendor class
        .bInterfaceSubClass = 0,
        .bInterfaceProtocol = 0,                        //vendor specific protocol
        .iInterface = STRINGS_DATA_INTERF_ID,
        .endpoint = comm_endpoint_descs,
        .extra = NULL,
        .extralen = 0,
    },
};


static const struct usb_interface usb_interfaces[] = 
{
    {
        .cur_altsetting = NULL,
        .num_altsetting = sizeof(comm_interf_descs) / sizeof(comm_interf_descs[0]),
        .iface_assoc = NULL,
        .altsetting = comm_interf_descs,
    },
};


const struct usb_config_descriptor usb_config_descs[] = 
{
    {
        .bLength = USB_DT_CONFIGURATION_SIZE,
        .bDescriptorType = USB_DT_CONFIGURATION,
        .wTotalLength = 0,
        .bNumInterfaces = sizeof(usb_interfaces) / sizeof(usb_interfaces[0]),
        .bConfigurationValue = 1,
        .iConfiguration = 0,
        .bmAttributes = 0x80,                           //bus-powered
        .bMaxPower = 0xfa,                              //500 mA
        .interface = usb_interfaces,
    },
};


const struct usb_device_descriptor usb_device_desc = 
{
    .bLength = USB_DT_DEVICE_SIZE,
    .bDescriptorType = USB_DT_DEVICE,
    .bcdUSB = 0x0200,                                   //USB version 2.00
    .bDeviceClass = USB_CLASS_VENDOR,
    .bDeviceSubClass = 0,
    .bDeviceProtocol = 0,                               //no class specific protocol
    .bMaxPacketSize0 = BULK_MAX_PACKET_SIZE,
    .idVendor = USB_VENDOR_ID,
    .idProduct = USB_PRODUCT_ID,
    .bcdDevice = USB_DEVICE_RELEASE,
    .iManufacturer = STRINGS_MANUFACTURER_ID,
    .iProduct = STRINGS_PRODUCT_ID,
    .iSerialNumber = STRINGS_SERIAL_NUMBER_ID,
    .bNumConfigurations = sizeof(usb_config_descs) / sizeof(usb_config_descs[0]),
};
