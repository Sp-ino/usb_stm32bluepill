#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sat Jan  8 17:17:54 2022

@author: spino
"""

import usb.core
import array
import sys
import time


WRITE_EP = 0x01
READ_EP = 0x82
PACKET_SIZE_BYTES = 64
BUFFER_SIZE_PACKETS = 4
BUFFER_SIZE_BYTES = BUFFER_SIZE_PACKETS * PACKET_SIZE_BYTES
USB_TIMEOUT = 5000


#-----------------------------Initialization-----------------------------------
# find device
dev = usb.core.find(idVendor=0xcafe, idProduct=0xcafe)

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


#write initial data. It is an array of 128 bytes of value 0xFF
initial_data = array.array('B', [0x30 for i in range(0,2*PACKET_SIZE_BYTES)])
try:
    writelen = dev.write(WRITE_EP, initial_data, timeout = USB_TIMEOUT)
except: 
    print("USB initial write failed")


#------------------------------Main while loop---------------------------------
while True:
    data = input("Values to be written over USB: ")
    
    #send data
    try:
        writelen = dev.write(WRITE_EP, data, timeout = USB_TIMEOUT)
    except: 
        print("USB write failed")
        
    if writelen:
        print(writelen)
