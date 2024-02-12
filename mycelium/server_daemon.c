#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_CONNECTIONS 5
#define BUFFER_SIZE 1024


int main() {
  // Fork the process so that the parent can exit.
  pid_t pid = fork();
  if (pid < 0) {
    exit(EXIT_FAILURE);
  }

  // If we are the child process, continue.
  if (pid == 0) {
    // Create a new session so that the daemon is not associated with a terminal.
    setsid();

    // Close all open file descriptors.
    for (int i = 0; i < 1024; i++) {
      close(i);
    }

    // Open the standard output and error to /dev/null.
    open("/dev/null", O_RDWR);
    dup(0);
    dup(0);

    // Change the working directory to the root directory.
    chdir("/");

    // Start the daemon process.
    while (1) {
      // Do something useful.
      printf("Hello from the daemon!\n");
      sleep(100);
    }
  }

  // If we are the parent process, exit.
  exit(EXIT_SUCCESS);
}
