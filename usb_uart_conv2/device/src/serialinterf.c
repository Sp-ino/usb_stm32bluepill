/*
 * USB bulk transfer example with UART communication - 2
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
    uint8_t bytes[BULK_MAX_PACKET_SIZE];
    uint8_t data_size;
    bool is_there_data;
} databuf;

//This variables stores whether the device configuration has been performed or not
static bool is_configured = false;

//data buffer
static databuf rx_buffer;

// USB device instance
static usbd_device *usb_device;

// buffer for control requests
static uint8_t usbd_control_buffer[256];


/*-------------------------- Function definitions -------------------------------------*/

static void init_rx_buffer(void)
{
    for (int index = 0; index < BULK_MAX_PACKET_SIZE; index++)
    {
        rx_buffer.bytes[index] = 0x00;
    }
    rx_buffer.data_size = 0;
    rx_buffer.is_there_data = false;
}


static void usb_init(void) 
{
    rcc_periph_clock_enable(RCC_USB);

    rcc_periph_reset_pulse(RST_USB);

    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_10_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO12);
    gpio_clear(GPIOA, GPIO12);
    delay(80);

    init_rx_buffer();

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

    rx_len = usbd_ep_read_packet(usbd_dev, EP_DATA_OUT, rx_buffer.bytes,
                                sizeof(rx_buffer.bytes));

    rx_buffer.data_size = rx_len;

    /* if data has been received, set endpoint to NAK.
     * handle_main_tasks will handle data and set the endpoint back to
     * VALID.   
    */
    if (rx_len > 0)
    {
        usbd_ep_nak_set(usb_device, EP_DATA_OUT, 1);
        rx_buffer.is_there_data = true;
    }
}


void handle_main_tasks(void)
{
    uint8_t len;
    uint8_t tx_buffer[BULK_MAX_PACKET_SIZE];

    if (!is_configured || !rx_buffer.is_there_data)
    {
        return;
    }

    /* Copy rx buffer into tx buffer before setting endpoint to VALID
     * This should help to speed up things a little, because possible delays
     * related to the UART peripheral won't prevent the device from receiving
     * a new packet from the host.
    */
    for (uint8_t index = 0; index < BULK_MAX_PACKET_SIZE; index++)
    {
        tx_buffer[index] = rx_buffer.bytes[index];
    }
    
    //save data_size into len to pass it to uart_tx
    len = rx_buffer.data_size;

    //set is_there_data to false to avoid resending the old data the next time
    //this function is called
    rx_buffer.is_there_data = false;

    //set endpoint to VALID. Now new data can be received because
    //the previous data has been moved to tx_buffer and its size
    //has been memorized
    usbd_ep_nak_set(usb_device, EP_DATA_OUT, 0);

    //Send the conent of tx_buffer over UART
    uart_tx(tx_buffer, len);
}


void usb_lp_can_rx0_isr(void)
{
    //allow driver to do its operations
    usbd_poll(usb_device);
}
