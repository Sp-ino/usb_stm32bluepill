/*
 * UART to USB converter
 * 
 * Copyright (c) 2022 Valerio Spinogatti
 * Licensed under GNU license
 * 
 * userint.c
 */

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <stdlib.h>
#include "common.h"
#include "util.h"
#include "descriptor.h"
#include "serialinterf.h"
#include "uart.h"


//This variables stores whether the device configuration has been performed or not
static bool is_configured = false;

// USB device instance
static usbd_device *usb_device;

// buffer for control requests
static uint8_t usbd_control_buffer[256];

static uint8_t single_byte;


/*-------------------------- Function definitions -------------------------------------*/


static void usb_init(void) 
{
    rcc_periph_clock_enable(RCC_USB);

    rcc_periph_reset_pulse(RST_USB);

    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_10_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO12);
    gpio_clear(GPIOA, GPIO12);
    delay(80);

    single_byte = 0x00;

    // create USB device
    usb_device = usbd_init(&st_usbfs_v1_usb_driver, &usb_device_desc, usb_config_descs,
                        usb_strings, sizeof(usb_strings) / sizeof(usb_strings[0]),
                        usbd_control_buffer, sizeof(usbd_control_buffer));

    // Set callback for config calls
    usbd_register_set_config_callback(usb_device, usb_set_config);
    //register_wcid_desc(usb_device);

    // Enable interrupt
    nvic_set_priority(NVIC_USB_LP_CAN_RX0_IRQ, 2 << 6);

    nvic_enable_irq(NVIC_USB_LP_CAN_RX0_IRQ);
}


void serial_interf_init(void)
{
    usb_init();
    uart_init();
}


void usb_set_config(usbd_device *usbd_dev, uint16_t wValue __attribute__((unused)))
{
    usbd_ep_setup(usbd_dev,
                EP_DATA_IN,
                USB_ENDPOINT_ATTR_BULK,
                BULK_MAX_PACKET_SIZE,
                NULL);

    is_configured = true;
    led_blink();
}


static void uart_rx(void)
{
    single_byte = uart_getc();
}


static void usb_tx(void)
{
    usbd_ep_write_packet(usb_device, EP_DATA_IN, &single_byte, 1);
}


void handle_main_tasks(void)
{
    if (!is_configured)
    {
        return;
    }

    uart_rx();
    usb_tx();
}


void usb_lp_can_rx0_isr(void)
{
    //allow driver to do its operations
    usbd_poll(usb_device);
}
