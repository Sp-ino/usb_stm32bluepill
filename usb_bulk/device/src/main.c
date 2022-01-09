#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/usb/usbd.h>
#include <stdlib.h>
#include "usb_descriptor.h"
#include "common.h"


//static buffer rx_buffer;
// #define BUFFER_LEN_PACKETS 1
// #define BUFFER_LEN_BYTES (BUFFER_LEN_PACKETS * BULK_MAX_PACKET_SIZE)
#define BUFFER_LEN_BYTES 64

static uint8_t rx_buffer[BUFFER_LEN_BYTES];
static volatile bool is_configured = false;
static volatile bool is_data_received = false;

// USB device instance
static usbd_device *usb_device;

// buffer for control requests
static uint8_t usbd_control_buffer[256];


static void usb_set_config(usbd_device *usbd_dev, uint16_t wValue);
static void handle_bulk_rx_cb(usbd_device *usbd_dev, uint8_t ep __attribute__((unused)));


static void led_blink(void) 
{
    for (int i = 0; i < 8; i++) 
    {
        gpio_toggle(GPIOC, GPIO13);
        delay(200);
    }
}


static void sys_init(void) 
{
    rcc_clock_setup_in_hse_8mhz_out_72mhz();
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOC);

    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
    gpio_set(GPIOC, GPIO13);

    systick_init();
}


static void usb_init(void)
{
    rcc_periph_clock_enable(RCC_USB);

    rcc_periph_reset_pulse(RST_USB);

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
    //register_wcid_desc(usb_device);

    // Enable interrupt
    nvic_set_priority(NVIC_USB_LP_CAN_RX0_IRQ, 2 << 6);
    nvic_enable_irq(NVIC_USB_LP_CAN_RX0_IRQ);
}


void usb_set_config(usbd_device *usbd_dev, 
                    uint16_t wValue __attribute__((unused)))
{
    usbd_ep_setup(usbd_dev,
                EP_DATA_OUT,
                USB_ENDPOINT_ATTR_BULK,
                BULK_MAX_PACKET_SIZE,
                handle_bulk_rx_cb);

    usbd_ep_setup(usbd_dev,
                EP_DATA_IN,
                USB_ENDPOINT_ATTR_BULK,
                BULK_MAX_PACKET_SIZE,
                NULL);
                
    is_configured = true;
    led_blink();
}


//this callback function is called when a packet is received. It stores the packet inside the buffer and sets the 
static void handle_bulk_rx_cb(usbd_device *usbd_dev, uint8_t ep __attribute__((unused)))
{
    unsigned len = usbd_ep_read_packet(usbd_dev, EP_DATA_OUT, rx_buffer, sizeof(rx_buffer));
    if (len > 0) {
        is_data_received = true;
    }
    usbd_ep_nak_set(usbd_dev, EP_DATA_OUT, 1);
}


int main(void) 
{
    sys_init();
    usb_init();

    while(1) 
    {
        if (is_configured && is_data_received)
        {
            usbd_ep_write_packet(usb_device, EP_DATA_IN, rx_buffer, sizeof(rx_buffer));
            usbd_ep_nak_set(usb_device, EP_DATA_OUT, 0);
            is_data_received = false;
        }
    }

    return 0;
}


void usb_lp_can_rx0_isr(void)
{
    usbd_poll(usb_device);
}