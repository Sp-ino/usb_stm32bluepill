#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sat Jan  8 17:17:54 2022

@author: spino


USB bulk transfer example with UART communication
 
Copyright (c) 2022 Valerio Spinogatti
Licensed under GNU license
 
repeater.py
 
"""

import usb.core
import array
import sys
import time
#import numpy as np

WRITE_EP = 0x01
READ_EP = 0x82

data = array.array('B', [0xFF for i in range(0,64)])
# readarr = array.array('B', [0x00 for i in range(1, 128)])


# find device
dev = usb.core.find(idVendor=0xcafe, idProduct=0xcafe)

if dev is None:
    sys.exit("repeater: device not found")
    
dev.reset()

for config in dev:
    for interf_num in range(config.bNumInterfaces):
        if dev.is_kernel_driver_active(interf_num):
            try:
                dev.detach_kernel_driver(interf_num)
            except usb.core.USBError as e:
                sys.exit("Could not detatch kernel driver from interface({0}): {1}".format(interf_num, str(e)))
      
    
# set configuration
dev.set_configuration()

dev.write(WRITE_EP, data)
time.sleep(0.2)
readbuf = dev.read(READ_EP, 64)
print(readbuf)

while True:
    data = input("Valori da inviare: ")
    
    #the string must be sliced because it cannot exceed the packet length.
    #this is due to the fact that the buffer implemented within the firmware is
    #exactly one packet long, which implies that the command line output will be 
    #delayed by one packet if more than 64 bytes at a time are sent.
    try:
        writelen = dev.write(WRITE_EP, data) 
        print(writelen)
    except usb.USBError:
        try:
            readbuf = dev.read(READ_EP, 64)
            print(readbuf)
        except:
            print("Could not write")
        
    time.sleep(0.2)
    try:
        readbuf = dev.read(READ_EP, 64)
        print(readbuf)
    except:
        print("Could not read")
