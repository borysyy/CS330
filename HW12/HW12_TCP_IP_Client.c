/*
Written by: Slava Borysyuk
Course: CS330
12/07/2023
HW12_TCP_IP_Client
Output:
Connected to server
Client: hi
Server: hello

Client:
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8888
#define BUFFER_SIZE 1024

int main()
{
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Create a socket for communication
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket creation failed"); // Display error if socket creation fails
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;                     // IPv4 address family
    server_addr.sin_port = htons(PORT);                   // Set the port number
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server IP address

    // Connect to server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Connection failed"); // Display error if connection fails
        exit(EXIT_FAILURE);
    }

    printf("Connected to server\n");

    // Communication loop
    while (1)
    {
        printf("Client: ");
        fgets(buffer, BUFFER_SIZE, stdin); // Read user input

        // Send message to server
        send(client_socket, buffer, strlen(buffer), 0);

        memset(buffer, 0, BUFFER_SIZE); // Clear the buffer

        // Receive message from server
        if (recv(client_socket, buffer, BUFFER_SIZE, 0) == -1)
        {
            perror("Receive failed"); // Display error if receiving message fails
            exit(EXIT_FAILURE);
        }

        printf("Server: %s\n", buffer); // Display server's response
    }

    close(client_socket); // Close the socket connection

    return 0;
}
