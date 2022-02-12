"""
Script to read data from UART with FTDI

Copyright (c) 2022 Valerio Spinogatti
Licensed under GNU license

ftdi_read_uart.py
"""

from numpy import size
import pyftdi.serialext as pser
import pyftdi.ftdi as ftdi
import serial.serialutil


def print_devices():
    possible_ftdi_devices = ftdi.Ftdi.list_devices()
    print(f"\nFound {len(possible_ftdi_devices)} FTDI devices.")
    for n, dev in enumerate(possible_ftdi_devices):
        print(f"\nDevice {n}:")
        for key, elem in zip(dev[0]._fields, dev[0]):
            print(key, "=", elem)

    print("\n")


def main():
    BAUDRATE = 38400

    print_devices()

    try:
        ftdi_dev = pser.serial_for_url("ftdi:///1", baudrate = BAUDRATE)
    except serial.serialutil.SerialException as e:
        raise OSError(e)

    while True:
        received_bytes = ftdi_dev.read(8)
        print(received_bytes.decode('utf-8'))


if __name__ == "__main__":
    main()