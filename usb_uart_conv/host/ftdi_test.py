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


buffer = []
print(ftdi.Ftdi.list_devices())
try:
    ftdi_dev = pser.serial_for_url("ftdi:///1")
except serial.serialutil.SerialException as e:
    raise OSError(e)

while True:
    buffer = ftdi_dev.read(128)

    if len(buffer) > 0:
        print(bytes(buffer))
