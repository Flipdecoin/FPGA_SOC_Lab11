#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#define _BSD_SOURCE

#define RADIO_ADD               0x43C00000
#define RADIO_ADD_CONFIG_OFFSET 0x02
#define RADIO_FIFO_ADD          0x43C10000
#define RADIO_FIFO_COUNT_OFFSET 0x07
#define RADIO_FIFO_DATA_OFFSET  0x08

unsigned int * get_a_pointer(unsigned int phys_addr)
{
        int mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
        void *map_base = mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, phys_addr);
        unsigned int *radio_base = (unsigned int *)map_base;
        return (radio_base);
}

void FillPacket(unsigned int *RadioFifo, unsigned int *data)
{
    int numWordsRead = 0;

    // Wait for fifo to fill
    while(*(RadioFifo + RADIO_FIFO_COUNT_OFFSET) < 256);

    // Read 256 words
    for(int i = 0; i < 256;i++)
    {
        data[i] = *(RadioFifo + RADIO_FIFO_DATA_OFFSET);
    }
}

void SendPacket(unsigned int *data, char *ip)
{
	static uint32_t counter = 0;
	uint8_t messageData[1028] = {};

	memcpy(messageData+4,(uint8_t*)data,1024);

    // Create socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Define the server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(25344); // Set port number (e.g., 12345)
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid IP address");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Send the message
	((uint32_t*)messageData)[0] = htonl(counter++);
    ssize_t sent_bytes = sendto(sock, messageData, 1028, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (sent_bytes < 0) {
        perror("Failed to send message");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Close socket
    close(sock);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <IP_ADDRESS>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Enable Valid bit streaming
    unsigned int *Radio = get_a_pointer(RADIO_ADD);
    unsigned int *RadioFifo = get_a_pointer(RADIO_FIFO_ADD);
    *(Radio+RADIO_ADD_CONFIG_OFFSET) = 0x00000002;

    // Parse command-line arguments
    char *ip_address = argv[1];

    unsigned int data[256] = {};;

    while(1)
    {
        // Reset data buffer
        memset(data,0,sizeof(data));

        // Fill Data buffer from FIFO
        FillPacket(RadioFifo, data);

        // Send data over UDP
        SendPacket(data, ip_address);
    }
}
