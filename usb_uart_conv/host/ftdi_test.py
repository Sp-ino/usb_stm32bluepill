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


def main():
    buffer = []
    # print(ftdi.Ftdi.list_devices())
    try:
        ftdi_dev = pser.serial_for_url("ftdi:///1", baudrate = 38400)
    except serial.serialutil.SerialException as e:
        raise OSError(e)

    while True:
        received_bytes = ftdi_dev.read(8)
        print(received_bytes.decode('utf-8'))


if __name__ == "__main__":
    main()