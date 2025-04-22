#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#define _BSD_SOURCE

#define RADIO_ADD       0x43C00000
#define RADIO_FIFO_ADD  0x43C10000
#define RADIO_FIFO_RESET_OFFSET 0x06
#define RADIO_FIFO_COUNT_OFFSET 0x07
#define RADIO_FIFO_DATA_OFFSET  0x08

volatile unsigned int * get_a_pointer(unsigned int phys_addr)
{
        int mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
        void *map_base = mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, phys_addr);
        volatile unsigned int *radio_base = (volatile unsigned int *)map_base;
        return (radio_base);
}

void main()
{
    volatile unsigned int *RadioFifo = get_a_pointer(RADIO_FIFO_ADD);

    printf("I will now read 10 seconds of data!\r\n");
    int numBytesRead = 0;
    unsigned int data[256] = {};

        *(RadioFifo+RADIO_FIFO_RESET_OFFSET) = 0x000000A5;
        *(RadioFifo+RADIO_FIFO_RESET_OFFSET) = 0x00000000;

    while(numBytesRead < 480000)
    {
        if(*(RadioFifo+0x7) >= 256)
        {
            for(int i = 0; i < 256;i++)
            {
                data[i] = *(RadioFifo + 0x8);
            }
            numBytesRead += 256;
        }
    }

    printf("Finished!\r\n");
}

