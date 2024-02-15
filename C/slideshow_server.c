#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <dirent.h>

#define PORT 4000
#define IMAGE_DIR "images"

void send_image(int client_fd, const char *image_path) {
    FILE *image_file = fopen(image_path, "rb");
    if (image_file == NULL) {
        perror("Error opening image file");
        return;
    }

    char buffer[1024];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), image_file)) > 0) {
        if (send(client_fd, buffer, bytes_read, 0) != bytes_read) {
            perror("send failed");
            fclose(image_file);
            return;
        }
    }

    fclose(image_file);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Setup server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket to address
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        // Accept incoming connection
        if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        printf("Connection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Open directory
        DIR *dir = opendir(IMAGE_DIR);
        if (dir == NULL) {
            perror("Error opening image directory");
            exit(EXIT_FAILURE);
        }

        // Read directory and send images
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            // Filter out directories and non-image files
            if (entry->d_type == DT_REG) {
                char image_path[256];
                snprintf(image_path, sizeof(image_path), "%s/%s", IMAGE_DIR, entry->d_name);
                send_image(client_fd, image_path);
            }
        }

        closedir(dir);
        close(client_fd);
    }

    return 0;
}

