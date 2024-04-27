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
#define BUFFER_SIZE 1024

// Function to generate a random uppercase letter
char random_letter() {
    return 'A' + rand() % 26;
}

int main() {
    int client_socket;
    struct sockaddr_in server_address;
    // char packet[6] = {0}; // 5 bytes for random letters + 1 byte for null terminator
    char buffer[BUFFER_SIZE] = {0};
    srand(time(NULL)); // Seed the random number generator

    // Create socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
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

    // Continuously send and receive data
    while (1) {
        // Generate a random packet
        char packet[] = "print\0";

        // Send the packet
        if (send(client_socket, packet, strlen(packet), 0) == -1) {
            perror("Send failed");
            exit(EXIT_FAILURE);
        }
        printf("Sent packet: %s\n", packet);

        // Receive data from server
        ssize_t bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received == -1) {
            perror("Receive failed");
            exit(EXIT_FAILURE);
        } else if (bytes_received == 0) {
            printf("Server closed connection\n");
            break;
        } else {
            buffer[bytes_received] = '\0';
            printf("Received from server: %s\n", buffer);
        }

        // Sleep for some time before sending the next packet
        sleep(1);

        char packet1[] = "raven\0";

        // Send the packet
        if (send(client_socket, packet1, strlen(packet1), 0) == -1) {
            perror("Send failed");
            exit(EXIT_FAILURE);
        }
        printf("Sent packet: %s\n", packet1);

        // Receive data from server
        ssize_t bytes_received1 = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received1 == -1) {
            perror("Receive failed");
            exit(EXIT_FAILURE);
        } else if (bytes_received1 == 0) {
            printf("Server closed connection\n");
            break;
        } else {
            buffer[bytes_received1] = '\0';
            printf("Received from server: %s\n", buffer);
        }

        // Sleep for some time before sending the next packet
        sleep(1);

        char packet2[] = "rapid\0";

        // Send the packet
        if (send(client_socket, packet2, strlen(packet2), 0) == -1) {
            perror("Send failed");
            exit(EXIT_FAILURE);
        }
        printf("Sent packet: %s\n", packet2);

        // Receive data from server
        ssize_t bytes_received2 = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received2 == -1) {
            perror("Receive failed");
            exit(EXIT_FAILURE);
        } else if (bytes_received2 == 0) {
            printf("Server closed connection\n");
            break;
        } else {
            buffer[bytes_received2] = '\0';
            printf("Received from server: %s\n", buffer);
        }

        // Sleep for some time before sending the next packet
        sleep(1);
    }

    // Close the socket
    close(client_socket);

    return 0;
}
