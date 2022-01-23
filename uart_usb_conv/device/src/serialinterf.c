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


typedef struct
{
    uint8_t bytes[BUFFER_LEN_BYTES];
    uint8_t data_size;
    bool is_buffer_full;
} databuf;

//This variables stores whether the device configuration has been performed or not
static bool is_configured = false;

//data buffer
static databuf data_buffer;

// USB device instance
static usbd_device *usb_device;

// buffer for control requests
static uint8_t usbd_control_buffer[256];


/*-------------------------- Function definitions -------------------------------------*/

static void init_data_buffer(void)
{
    for (int index = 0; index < BUFFER_LEN_BYTES; index++)
    {
        data_buffer.bytes[index] = 0x00;
    }
    data_buffer.data_size = 0;
    data_buffer.is_buffer_full = false;
}


static void usb_init(void) 
{
    rcc_periph_clock_enable(RCC_USB);

    rcc_periph_reset_pulse(RST_USB);

    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_10_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO12);
    gpio_clear(GPIOA, GPIO12);
    delay(80);

    init_data_buffer();

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
    uint16_t datasize = data_buffer.data_size;

    if (data_buffer.is_buffer_full)
    {
        return;
    }

    data_buffer.bytes[datasize] = uart_getc();
    data_buffer.data_size++;

    if (data_buffer.data_size > BUFFER_LEN_BYTES)
    {
        data_buffer.data_size = true;
    }
}


static void usb_tx(void)
{
    uint8_t tx_buffer[BULK_MAX_PACKET_SIZE];

    if (!data_buffer.is_buffer_full)
    {
        return;
    }

    for (uint8_t packet_index = 0; packet_index < BUFFER_LEN_PACKETS; packet_index++)
    {
        for (uint8_t index = 0; index < BULK_MAX_PACKET_SIZE; index++)
        {
            tx_buffer[index] = data_buffer.bytes[packet_index * BULK_MAX_PACKET_SIZE + index];
        }

        usbd_ep_write_packet(usb_device, EP_DATA_IN, tx_buffer, BULK_MAX_PACKET_SIZE);
    }

    data_buffer.data_size = 0;
    data_buffer.is_buffer_full = false;
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
