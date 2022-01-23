/*
 * USB bulk transfer example with UART communication
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
    bool is_full;
} databuf;

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
    data_buffer.is_full = false;
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
                EP_DATA_OUT,
                USB_ENDPOINT_ATTR_BULK,
                BULK_MAX_PACKET_SIZE,
                handle_usb_packet_rx_cb);

    is_configured = true;
    led_blink();
}


void handle_usb_packet_rx_cb(usbd_device *usbd_dev, uint8_t ep __attribute__((unused)))
{
    uint16_t rx_len;
    uint8_t temp_buffer[BULK_MAX_PACKET_SIZE];

    rx_len = usbd_ep_read_packet(usbd_dev, EP_DATA_OUT, temp_buffer, sizeof(temp_buffer));

    if (rx_len > 0) 
    {
        for(uint8_t index = data_buffer.data_size; index < data_buffer.data_size + rx_len; index++)
        {
            data_buffer.bytes[index] = temp_buffer[index - data_buffer.data_size];
        }

        data_buffer.data_size += rx_len;

        if (data_buffer.data_size > BUFFER_MAX_OCCUPIED_SIZE) 
        {
            data_buffer.is_full = true;
            usbd_ep_nak_set(usb_device, EP_DATA_OUT, 1);
        }
    }
}


void handle_main_tasks(void)
{
    uint8_t tx_buffer[TX_BUFFER_LEN_BYTES];

    //return if usb configuration has not been performed or data buffer is not full
    if (!is_configured || !data_buffer.is_full)
    {
        return;
    }

    //copy bytes from 0 to TX_BUFFER_LEN_BYTES (length of tx buffer) 
    //into tx buffer
    for(uint16_t index = 0; index < TX_BUFFER_LEN_BYTES; index++)
    {
        tx_buffer[index] = data_buffer.bytes[index];
    }

    //send content of tx buffer over UART
    uart_tx(tx_buffer, TX_BUFFER_LEN_BYTES);

    //shift all buffer by TX_BUFFER_LEN_BYTES positions toward left
    for(uint16_t index = TX_BUFFER_LEN_BYTES; index < data_buffer.data_size; index++)
    {
        data_buffer.bytes[index - TX_BUFFER_LEN_BYTES] = data_buffer.bytes[index];
    }

    //decrease data_buffer.data_size by length of tx buffer
    data_buffer.data_size -= TX_BUFFER_LEN_BYTES;

    //set is_full member to false and endpoint to VALID
    data_buffer.is_full = false;
    usbd_ep_nak_set(usb_device, EP_DATA_OUT, 0);
}


void usb_lp_can_rx0_isr(void)
{
    //allow driver to do its operations
    usbd_poll(usb_device);
}
