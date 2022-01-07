#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usbd.h>
#include "wcid.h"
#include "usb_descriptor.h"
#include "buffer.h"


static buffer data_buffer;

static void usb_set_config(usbd_device *usbd_dev, uint16_t wValue);
static void handle_bulk_rx_cb(usbd_device *usbd_dev, uint8_t ep __attribute__((unused)));


void init_sys() 
{
    rcc_clock_setup_in_hse_8mhz_out_72mhz();
    rcc_periph_clock_enable(RCC_GPIOC);

    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
    gpio_clear(GPIOC, GPIO13)

    systick_init();
}


void usb_init()
{
    rcc_periph_clock_enable(RCC_USB);

    rcc_periph_reset_pulse(RCC_USB);

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


void usb_set_config(usbd_device *usbd_dev, 
                    uint16_t wValue __attribute__((unused)))
{
    usbd_ep_setup(usbd_dev,
                EP_DATA_OUT,
                USB_ENDPOINT_ATTR_BULK,
                BUFFER_LEN,
                handle_bulk_rx_cb);

    usbd_ep_setup(usbd_dev,
                EP_DATA_OUT,
                USB_ENDPOINT_ATTR_BULK,
                BUFFER_LEN,
                NULL);
}


static void handle_bulk_rx_cb(usbd_device *usbd_dev, uint8_t ep __attribute__((unused)))
{

}