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
    uint8_t bytes[BULK_MAX_PACKET_SIZE];
    uint8_t data_size;
} databuf;

static uint8_t write_buf_index = 0;
static uint8_t read_buf_index = 0;
static uint8_t n_full_buf = 0;

static bool is_configured = false;

//ping-pong buffer
static databuf pingpong_buffer[PINGPONG_BUF_SIZE];

// USB device instance
static usbd_device *usb_device;

// buffer for control requests
static uint8_t usbd_control_buffer[256];


/*-------------------------- Function definitions -------------------------------------*/

static void init_pingpong_buffer(void)
{
    for (int index = 0; index < PINGPONG_BUF_SIZE; index++)
    {
        for (int bytenum = 0; bytenum < BULK_MAX_PACKET_SIZE; bytenum++)
        {
            pingpong_buffer[index].bytes[bytenum] = 0x00;
        }
        pingpong_buffer[index].data_size = 0;
    }
}


static void usb_init(void) 
{
    rcc_periph_clock_enable(RCC_USB);

    rcc_periph_reset_pulse(RST_USB);

    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_10_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO12);
    gpio_clear(GPIOA, GPIO12);
    delay(80);

    init_pingpong_buffer();

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

    //if both buffers are full then return because no data can be received. The host will send 
    //data again later
    if (n_full_buf == 2)
    {
        return;
    }

    /* 
     * not sure why this function should not work as it is, but in case it doesn't, I might try this:
     *
     * n_full_buf++;
     * 
     * then, after usbd_ep_read_packet (which sets endpoint to VALID)
     * 
     * if (rx_len > 0)
     * {
     *      //toggle write_buf_index
     *      write_buf_index = (uint8_t)!((bool)write_buf_index);
     * 
     *      //n_full_buf is kept as it is, because data has been received and the 
     *      //a priori decision to increment it was correct 
     * } 
     * else
     * {
     *      //do not toggle write_buf_index and decrement n_full_buf
     *      //because no data has been received
     *      n_full_buf--;
     * }
    */

   //read packet and set endpoint to VALID
    rx_len = usbd_ep_read_packet(usbd_dev, EP_DATA_OUT, pingpong_buffer[write_buf_index].bytes,
                                sizeof(pingpong_buffer[write_buf_index].bytes));

    //return if no data has been received
    if (rx_len == 0)
    {
        return; 
    }  

    pingpong_buffer[write_buf_index].data_size = rx_len;
    
    //increment n_full_buf
    n_full_buf++;

    if (n_full_buf == 1)
    {
        read_buf_index = write_buf_index;
    }

    //put write_buf_index to 1 if it is 0 and viceversa to point to the other buffer
    write_buf_index = (uint8_t)!((bool)write_buf_index);    
}


void handle_main_tasks(void)
{
    uint8_t index;

    if (!is_configured || n_full_buf == 0)
    {
        return;
    }

    if (pingpong_buffer[read_buf_index].data_size > 0)
    {
        index = pingpong_buffer[read_buf_index].data_size - 1;

        uart_tx(pingpong_buffer[read_buf_index].bytes[index]); 

        pingpong_buffer[read_buf_index].data_size--;
    }

    if (pingpong_buffer[read_buf_index].data_size == 0)
    {
        uart_tx('\r');
        uart_tx('\n');        

        read_buf_index = (uint8_t)!((bool)read_buf_index);
        
        n_full_buf--;
    }
}


void usb_lp_can_rx0_isr(void)
{
    //allow driver to do its operations
    usbd_poll(usb_device);
}