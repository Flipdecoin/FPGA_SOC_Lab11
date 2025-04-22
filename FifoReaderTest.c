#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h> 
#include <fcntl.h> 
#include <unistd.h>
#define _BSD_SOURCE

#define RADIO_ADD       0x43C00000
#define RADIO_FIFO_ADD  0x43C10000
#define RADIO_FIFO_COUNT_OFFSET 0x1C
#define RADIO_FIFO_DATA_OFFSET  0x20

volatile unsigned int * get_a_pointer(unsigned int phys_addr)
{
	int mem_fd = open("/dev/mem", O_RDWR | O_SYNC); 
	void *map_base = mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, phys_addr); 
	volatile unsigned int *radio_base = (volatile unsigned int *)map_base; 
	return (radio_base);
}

void main()
{
    volatile unsigned int *RadioPtr = get_a_pointer(RADIO_ADD);
    volatile unsigned int *RadioDataCountPtr = get_a_pointer(RADIO_FIFO_ADD + RADIO_FIFO_COUNT_OFFSET);
    volatile unsigned int *RadioDataPtr = get_a_pointer(RADIO_FIFO_ADD + RADIO_FIFO_DATA_OFFSET);

    printf("I will now read 10 seconds of data!\r\n");
    int numBytesRead = 0;
    unsigned int data[256] = {};
    while(numBytesRead < 480000)
    {
        if(*RadioDataCountPtr >= 256)
        {
            for(int i = 0; i < 256;i++)
            {
                data(i) = *RadioDataPtr;
            }
            numBytesRead += 256;
        }
    }

    printf("Done Reading!\r\n");
}