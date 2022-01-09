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
#import numpy as np

WRITE_EP = 0x01
READ_EP = 0x81

data = array.array('B', [0xFF for i in range(1,64)])
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
time.sleep(1)
readbuf = dev.read(READ_EP, 64)
print(readbuf)

while True:
    data = input("Valori da inviare: ")
    writelen = dev.write(WRITE_EP, data)
    print(writelen)
    time.sleep(1)
    readbuf = dev.read(READ_EP, 64)
    print(readbuf)
