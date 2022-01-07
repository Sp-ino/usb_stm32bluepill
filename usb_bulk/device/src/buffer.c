#include "buffer.h"


//Function for resetting the buffer
void reset_buffer(buffer *buf)
{
    buf->free_space = BUFFER_SIZE;

    for(uint8_t index = 0; index < BUFFER_SIZE; index++)
    {
        buf->bytes[index] = 0x00;
    }
}

 //Function for writing to the buffer
void write_buffer(buffer *buf, uint8_t **packet, uint16_t write_size)
{
    int start_index;
    int last_index;

    start_index = BUFFER_SIZE - buf->free_space;

    if (write_size > buf->free_space)
    {
        last_index = BUFFER_SIZE;
    }
    else
    {
        last_index = BUFFER_SIZE - (buf->free_space - write_size); 
    }

    for(uint8_t index = start_index; index < last_index; index++) 
    {
        buf->bytes[index] = (*packet)[index];
        buf->free_space--;
    }
}