#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

#define SHM_SIZE 1024
#define SEM_KEY 1234

struct sembuf acquire = {0, -1, SEM_UNDO};
struct sembuf release = {0, 1, SEM_UNDO};

int main() {
    int shmid, semid;
    char *shm_ptr;
    key_t key = ftok("/home/pi/mycelium", 'm');
    printf("key = %d\n", key);

    // Attach shared memory
    if ((shmid = shmget(key, SHM_SIZE, 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    // Attach shared memory
    if ((shm_ptr = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }

    // Get semaphore
    if ((semid = semget(SEM_KEY, 1, 0666)) == -1) {
        perror("semget");
        exit(1);
    }

    while (1) {
        char message[SHM_SIZE];

        printf("Enter message to send (or 'quit' to exit): ");
        fgets(message, SHM_SIZE, stdin);
        message[strcspn(message, "\n")] = 0;
        if (strcmp(message, "quit") == 0) break;
	
	while (*shm_ptr != '\0') usleep(100000);

        semop(semid, &acquire, 1);
        strncpy(shm_ptr, message, SHM_SIZE);
        semop(semid, &release, 1);
        if (strcmp(message, "exit") == 0) break;
    }

    shmdt(shm_ptr);

    return 0;
}
