#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8192
#define PACKET_SIZE 5

// Function to generate a random uppercase letter
char random_letter() {
    return 'A' + rand() % 26;
}

int main() {
    int client_socket;
    struct sockaddr_in server_address;
    char packet[PACKET_SIZE + 1]; // +1 for null terminator
    srand(time(NULL)); // Seed the random number generator

    // Create socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    memset(&server_address, '0', sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // Send packets periodically
    while (1) {
        // Generate a random packet
        for (int i = 0; i < PACKET_SIZE; ++i) {
            packet[i] = random_letter();
        }
        packet[PACKET_SIZE] = '\0'; // Null terminate the string

        // Send the packet
        if (send(client_socket, packet, strlen(packet), 0) != strlen(packet)) {
            perror("Send failed");
            exit(EXIT_FAILURE);
        }

        printf("Sent packet: %s\n", packet);

        // Sleep for some time before sending the next packet
        sleep(1);
    }

    // Close socket
    close(client_socket);

    return 0;
}
