#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sat Jan  8 17:17:54 2022

@author: spino


UART to USB converter
 
Copyright (c) 2022 Valerio Spinogatti
Licensed under GNU license
 
repeater.py
"""

import usb.core
import array
import sys
#import time


#constant definitions
READ_EP = 0x82
PACKET_SIZE_BYTES = 64
BUFFER_SIZE_PACKETS = 2
BUFFER_SIZE_BYTES = BUFFER_SIZE_PACKETS * PACKET_SIZE_BYTES
USB_TIMEOUT = 5000


#-----------------------------Initialization-----------------------------------
# find device
dev = usb.core.find(idVendor=0x0298, idProduct=0x0298)

if dev is None:
    sys.exit("repeater: device not found")

#reset device. necessary to avoid driver conflicts that occur when this application
#is stopped and then run again without resetting the device
dev.reset()

#detach kernel driver
for config in dev:
    for interf_num in range(config.bNumInterfaces):
        if dev.is_kernel_driver_active(interf_num):
            try:
                dev.detach_kernel_driver(interf_num)
            except usb.core.USBError as e:
                sys.exit("Could not detatch kernel driver from interface({0}): {1}".format(interf_num, str(e)))    
    
# set configuration
dev.set_configuration()

rx_buffer = array.array('b', [0x00 for i in range(0, BUFFER_SIZE_BYTES)])
readlen = None


#------------------------------Main while loop---------------------------------
while True:

    #read data
    try:
        readlen = dev.read(READ_EP, rx_buffer, timeout = USB_TIMEOUT)
    except: 
        pass

    if readlen:
        if readlen > 0:
            print(rx_buffer.tobytes())