#include "buffer.h"


//Function for resetting the buffer
void reset_buffer(buffer *buf)
{
    buf->free_space = BUFFER_SIZE;
}

 //Function for writing to the buffer
void write_buffer(buffer *buf, uint8_t *packet, uint16_t write_size)
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
        buf->bytes[index] = packet[index];
        buf->free_space--;
    }
}


void read_buffer(buffer *buf, uint8_t *packet, uint16_t read_size)
{
    if (read_size > BUFFER_SIZE)
    {
        for (uint16_t index = 0; index < BUFFER_SIZE; index ++)
        {
            packet[index] = buf->bytes[index];
        }
        buf->free_space = BUFFER_SIZE;
    }

    else
    {
        for (uint16_t index = 0; index < read_size; index ++)
        {
            packet[index] = buf->bytes[index];
        }
        for (uint16_t index = read_size; index < BUFFER_SIZE - buf->free_space; index ++)
        {
            buf->bytes[index - read_size] = buf->bytes[index];
        }
        buf->free_space = buf->free_space + read_size;
    }
}