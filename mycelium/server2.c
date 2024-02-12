// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void send_response(int client_socket, const char *status, const char *content_type, const char *content) {
    char response[BUFFER_SIZE];
    sprintf(response, "HTTP/1.1 %s\r\nContent-Type: %s\r\n\r\n%s", status, content_type, content);
    send(client_socket, response, strlen(response), 0);
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attach socket to the port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Bind socket to address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    // Accept incoming connections
    while (1) {
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // Read HTTP request
        read(client_socket, buffer, BUFFER_SIZE);

        // Extract requested file path (for simplicity, assume all requests are for images in the 'images' directory)
        char requested_file[200] = "images/";
        char requested_path[BUFFER_SIZE];
        sscanf(buffer, "GET %s HTTP", requested_path);
        strcat(requested_file, requested_path);
	printf(requested_file);
        // Open the requested file
        FILE *fp = fopen(requested_file, "rb");
        if (fp == NULL) {
            // File not found, send a 404 response
            send_response(client_socket, "404 Not Found", "text/html", "<h1>404 Not Found</h1>");
        } else {
            // File found, send the file content with appropriate content type
            fseek(fp, 0L, SEEK_END);
            long file_size = ftell(fp);
            fseek(fp, 0L, SEEK_SET);

            char *file_contents = (char *)malloc(file_size);
            fread(file_contents, file_size, 1, fp);

            fclose(fp);

            // Determine the content type based on file extension (for simplicity, assume images are JPEGs)
            send_response(client_socket, "200 OK", "image/jpg", file_contents);

            free(file_contents);
        }

        // Close the client socket
        close(client_socket);
    }
    return 0;
}

