#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	static uint32_t counter = 0;
	uint8_t messageData[1028] = {};
	uint16_t complexData[512] = {};

	memcpy(messageData+4,(uint8_t*)complexData,1024);
	
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <IP_ADDRESS> <NUMBER_OF_PACKETS>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Parse command-line arguments
    char *ip_address = argv[1];
    int num_packets = atoi(argv[2]);
    if (num_packets <= 0) {
        fprintf(stderr, "Error: Number of packets must be greater than 0.\n");
        exit(EXIT_FAILURE);
    }

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
    if (inet_pton(AF_INET, ip_address, &server_addr.sin_addr) <= 0) {
        perror("Invalid IP address");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Send the message
    for (int i = 0; i < num_packets; i++) {
	((uint32_t*)messageData)[0] = htonl(counter++);
        ssize_t sent_bytes = sendto(sock, messageData, 1028, 0,
                                    (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (sent_bytes < 0) {
            perror("Failed to send message");
            close(sock);
            exit(EXIT_FAILURE);
        }
        //printf("Packet %d sent successfully!\n", i + 1);
    }
	printf("%d Packets Sent Successfully\r\n", num_packets);

    // Close socket
    close(sock);

    return 0;
}
