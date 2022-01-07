#include <libopencm3/stm32/gpio.h>
#define BUFFER_SIZE 1024


//This struct represents the buffer for storing data coming from the host.
//Write and reset operations to the buffer must be exclusively handled by
//means of the reset_buffer() and write_buffer() functions.
typedef struct
{
	uint8_t bytes[BUFFER_SIZE];

	uint16_t free_space;

} buffer;


void reset_buffer(buffer *buf);

void write_buffer(buffer *buf, uint8_t **packet, uint16_t write_size);
