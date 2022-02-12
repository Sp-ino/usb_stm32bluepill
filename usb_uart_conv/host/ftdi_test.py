"""
The purpose of this module is to test the bluepill-based UART converter
with the FTDI board.

USB bulk transfer example with UART communication

Copyright (c) 2022 Valerio Spinogatti
Licensed under GNU license

ftdi_test.py
"""

import pyftdi.serialext as pser
import pyftdi.ftdi as ftdi
import serial.serialutil
import time


buffer = []
print(ftdi.Ftdi.list_devices())
try:
    ftdi_dev = pser.serial_for_url("ftdi:///1", baudrate = 38400)
except serial.serialutil.SerialException as e:
    raise OSError(e)

while True:
    buffer = ftdi_dev.read(16)

    print(buffer)

    # print("Buffering     ", end = '\r')
    # time.sleep(0.5)
    # print("Buffering .   ", end = '\r')
    # time.sleep(0.5)
    # print("\rBuffering .. ", end  = '\r')
    # time.sleep(0.5)
    # print("\rBuffering ...", end = '\r')
    # time.sleep(0.5)