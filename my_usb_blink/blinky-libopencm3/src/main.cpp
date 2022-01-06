/*
 * USB Tutorial
 * 
 * Copyright (c) 2020 Manuel Bleichenbacher
 * Licensed under MIT License
 * https://opensource.org/licenses/MIT
 * 
 * Main program
 */

#include "common.h"
#include "usb_descriptor.h"
#include "wcid.h"
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/usb/usbd.h>

static void usb_set_config(usbd_device *usbd_dev, uint16_t wValue);
static usbd_request_return_codes handle_led_req(usbd_device *usbd_dev, usb_setup_data *req,
                                             uint8_t **buf, uint16_t *len,
                                             usbd_control_complete_callback *complete);
static usbd_request_return_codes handle_data_req(usbd_device *usbd_dev, usb_setup_data *req,
                                             uint8_t **buf, uint16_t *len,
                                             usbd_control_complete_callback *complete);

// USB device instance
usbd_device *usb_device;

// buffer for control requests
uint8_t usbd_control_buffer[256];


void init()
{
    // Enable required clocks
    rcc_clock_setup_in_hse_8mhz_out_72mhz();
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOC);
    rcc_periph_clock_enable(RCC_GPIOB);

    // Initialize LED
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_10_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
    gpio_set(GPIOC, GPIO13);

    //Initialize external LED
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO0);
    gpio_clear(GPIOA, GPIO0);

    //Initialize pushbutton pin
    gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO8);
    gpio_clear(GPIOC, GPIO8);

    // Initialize systick services
    systick_init();
}

void usb_init()
{
    // enable USB clock
    rcc_periph_clock_enable(RCC_USB);

    // reset USB peripheral
    rcc_periph_reset_pulse(RST_USB);

    // Pull USB D+ (A12) low for 80ms to trigger device reenumeration
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_10_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO12);
    gpio_clear(GPIOA, GPIO12);
    delay(80);

    usb_init_serial_num();

    // create USB device
    usb_device = usbd_init(&st_usbfs_v1_usb_driver, &usb_device_desc, usb_config_descs,
                           usb_desc_strings, sizeof(usb_desc_strings) / sizeof(usb_desc_strings[0]),
                           usbd_control_buffer, sizeof(usbd_control_buffer));

    // Set callback for config calls
    usbd_register_set_config_callback(usb_device, usb_set_config);
    register_wcid_desc(usb_device);

    // Enable interrupt
    nvic_set_priority(NVIC_USB_LP_CAN_RX0_IRQ, 2 << 6);
    nvic_enable_irq(NVIC_USB_LP_CAN_RX0_IRQ);
}

// Called when the host connects to the device and selects a configuration
void usb_set_config(usbd_device *usbd_dev, __attribute__((unused)) uint16_t wValue)
{
    register_wcid_desc(usbd_dev);
    usbd_register_control_callback(usbd_dev,
                                   USB_REQ_TYPE_VENDOR | USB_REQ_TYPE_INTERFACE,
                                   USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
                                   handle_led_req);
    usbd_register_control_callback(usbd_dev,
                                   USB_REQ_TYPE_IN | USB_REQ_TYPE_VENDOR | USB_REQ_TYPE_INTERFACE,
                                   USB_REQ_TYPE_DIRECTION | USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
                                   handle_data_req);
}


// Called when a vendor request has been received
usbd_request_return_codes handle_led_req(__attribute__((unused)) usbd_device *usbd_dev, usb_setup_data *req,
                                      uint8_t **buf, uint16_t *len,
                                      __attribute__((unused)) usbd_control_complete_callback *complete)
{
    // The expected request format is (bmRequestType is filtered by callback registration):
    // bmRequestType = 0x41 (data direction: host to device, type: vendor, recipient: interface)
    // bmRequest: 0x33 (LED control request)
    // wValue: 0 for LED off, 1 for LED on
    // wIndex: 0 (interface number)
    if (req->bRequest == LED_CONTROL_ID && req->wIndex == 0)
    {
        switch(req->wValue)
        {
            //built-in LED OFF
            case 0:
                gpio_set(GPIOC, GPIO13);
                // no data in response
                *buf = nullptr;
                *len = 0;
                return USBD_REQ_HANDLED;
                break;

            //built-in LED ON 
            case 1:
                gpio_clear(GPIOC, GPIO13);
                // no data in response
                *buf = nullptr;
                *len = 0;
                return USBD_REQ_HANDLED;
                break;

            //LED on PA0 OFF
            case 2:
                gpio_clear(GPIOA, GPIO0);
                *buf = nullptr;
                *len = 0;
                return USBD_REQ_HANDLED;
                break;

            //LED on PA0 ON
            case 3:
                gpio_set(GPIOA, GPIO0);
                *buf = nullptr;
                *len = 0;
                return USBD_REQ_HANDLED;
                break;
        }
    }
    //if bRequest and wIndex fields do not match, pass on to next request handler
    return USBD_REQ_NEXT_CALLBACK;
}


usbd_request_return_codes handle_data_req(__attribute__((unused)) usbd_device *usbd_dev, usb_setup_data *req,
                                    uint8_t **buf __attribute__((unused)), uint16_t *len,
                                    __attribute__((unused)) usbd_control_complete_callback *complete)
{
    if (req->bRequest == PUSHBUTTON_REQ_ID && req->wIndex == 0)
    {
        bool pushbutton_state = gpio_get(GPIOB, GPIO8);
        uint8_t N_BYTES = 1;
        if(pushbutton_state == 1) 
        {
            usbd_control_buffer[0] = 0x01;
        }
        else  
        {
            usbd_control_buffer[0] = 0x00;
        }
        *len = N_BYTES;
        return USBD_REQ_HANDLED;
    }

    // pass on to next request handler
    return USBD_REQ_NEXT_CALLBACK;
}


int main()
{
    init();
    usb_init();

    while (true)
        ; // do nothing; all the action is in interrupt handlers
}

// USB interrupt handler
extern "C" void usb_lp_can_rx0_isr()
{
    usbd_poll(usb_device);
}
