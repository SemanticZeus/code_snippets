#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define SHM_SIZE 1024
#define SEM_KEY 1234

#define MAX_CLIENTS 100
#define PORT 1985
#define BUFFER_SIZE 1024
int server_running = 1;

struct sembuf acquire = {0, -1, SEM_UNDO};
struct sembuf release = {0, 1, SEM_UNDO};
int shmid, semid;
char *shm_ptr;
key_t key;
int active = 0;
double time_interval = 60;

void signal_handler(int sig) {
	switch(sig) {
	case SIGTERM:
		syslog(LOG_INFO, "Received SIGTERM, shutting down...");
		// Perform cleanup and shutdown procedures
		// Detach shared memory
		shmdt(shm_ptr);
		// Remove shared memory
		shmctl(shmid, IPC_RMID, NULL);
		server_running = 0;
		exit(EXIT_SUCCESS);
		break;
	case SIGHUP:
		syslog(LOG_INFO, "Received SIGHUP, reloading configuration...");
		// Reload configuration files or settings
		break;
		// Handle other signals as needed
	}
}

void *handle_client(void *arg) {
	int client_socket = *(int *)arg;
	char webpage[1024]; 
	char http_response[1024];
	sprintf(webpage, "<html><head><title>Sample Webpage</title></head><body><h1>Welcome to My Webpage!</h1><h1>Capturing Image = %d, time_interval = %d</h1></body></html>", active, (int)time_interval);
	sprintf(http_response, "HTTP/1.1 200 OK\nContent-Length: %ld\nContent-Type: text/html\n\n%s",
	        strlen(webpage), webpage);
	send(client_socket, http_response, strlen(http_response), 0);
	close(client_socket);
	pthread_exit(NULL);
}

void *server(void *unused) {
	int server_fd, new_socket;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[BUFFER_SIZE] = {0};
	pthread_t tid[MAX_CLIENTS];

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0))<0) goto SERVER_EXIT;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))<0) {
		perror("setsockopt");
		goto SERVER_EXIT;
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) goto SERVER_EXIT;
	if (listen(server_fd, 3) < 0) goto SERVER_EXIT;

	int flags = fcntl(server_fd, F_GETFL, 0);
	flags = flags | O_NONBLOCK;
	fcntl(server_fd, F_SETFL, flags);

	int client_index = 0;
	while (server_running) {
		int valread;
		usleep(100000);
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
			continue;
		if (pthread_create(&tid[client_index], NULL, handle_client, (void *)&new_socket) != 0)
			continue;
		pthread_detach(tid[client_index]);
		client_index++;
		if (client_index >= MAX_CLIENTS) {
			for (int k=0; k<MAX_CLIENTS; k++) pthread_join(tid[k], NULL);
			client_index = 0;
		}
	}
SERVER_EXIT:
	close(server_fd);
	pthread_exit(NULL);
}

void close_previous_daemon()
{
	struct sembuf acquire = {0, -1, SEM_UNDO};
	struct sembuf release = {0, 1, SEM_UNDO};
	int shmid, semid;
	char *shm_ptr;
	key_t key = ftok("mycelium", 'm');
	
	if ((shmid = shmget(key, SHM_SIZE, 0666)) < 0) return;
	if ((shm_ptr = shmat(shmid, NULL, 0)) == (char *) -1) return;
	if ((semid = semget(SEM_KEY, 1, 0666)) == -1) return;

	while (*shm_ptr != '\0') usleep(100000);

	semop(semid, &acquire, 1);
	strncpy(shm_ptr, "exit", SHM_SIZE);
	semop(semid, &release, 1);
	shmdt(shm_ptr);
	usleep(100000);
}

pid_t pid, sid;

void daemonize()
{
	// Fork off the parent process
	pid = fork();
	if (pid < 0) {
		exit(EXIT_FAILURE);
	}
	if (pid > 0) {
		exit(EXIT_SUCCESS); // Parent exits
	}

	// Change the file mode mask
	umask(0);

	// Create a new SID for the child process
	sid = setsid();
	if (sid < 0) {
		exit(EXIT_FAILURE);
	}
	close_previous_daemon();
	key = ftok("/home/pi/mycelium", 'm');
	printf("key = %d\n", key);

	if ((shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666)) < 0) {
		perror("shmget");
		exit(1);
	}

	printf("shmid = %d\n", shmid);
	
	// Attach shared memory
	if ((shm_ptr = shmat(shmid, NULL, 0)) == (char *) -1) {
		perror("shmat");
		exit(1);
	}

	if ((semid = semget(SEM_KEY, 1, IPC_CREAT | 0666)) == -1) {
		perror("semget");
		exit(1);
	}
	
	printf("semid = %d\n", semid);
	// Initialize semaphore
	semctl(semid, 0, SETVAL, 1);

	// Set signal handlers
	signal(SIGTERM, signal_handler);
	signal(SIGHUP, signal_handler);

	// Close standard file descriptors
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	chdir("/home/pi/");
}

void read_config()
{
    FILE *file;
    char line[1024];
    file = fopen("/home/pi/config.txt", "r");
    if (file == NULL) return;
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "active=1", 7) == 0) {
            active = 1;
        } else if (strncmp(line, "active=0", 7) == 0) {
            active = 0;
        } else if (strncmp(line, "time_interval=", 13) == 0) {
		int a = atoi(line+14);
		if (a<10 || a>5*60) time_interval = 60;
	       	else time_interval = a;
	}	
    }
    fclose(file);
}

int main() {
	time_t previous_time, current_time;
	double seconds_passed;
	previous_time = 0;

    	system("/usr/bin/python /home/pi/send_ip_address_by_email.py");
	daemonize();
	pthread_t server_thread_id;
	if (pthread_create(&server_thread_id, NULL, server, NULL)) printf("error, creating the server");

	//read_config();
	while (1) {
		read_config();
		// Wait for semaphore
		semop(semid, &acquire, 1);
		// Read message from shared memory
		if(strcmp(shm_ptr, "exit")==0) {
			break;
		} else if(strcmp(shm_ptr, "start")==0) {
			active = 1;
		} else if(strcmp(shm_ptr, "stop")==0) {
			active = 0;
		}
		if (strlen(shm_ptr)) {
			//FILE *f = fopen("./daemon.txt", "a");
			//fprintf(f, "message =  %s\n", shm_ptr);
			//fclose(f);
			*shm_ptr = '\0';
		}
		semop(semid, &release, 1);
		time(&current_time);
		if(difftime(current_time, previous_time)>time_interval) {
			previous_time = current_time;
			//if (!pause) system("python image.py");
			if (active) system("/home/pi/capture_image");
		}

		usleep(1000);
	}
	shmdt(shm_ptr);
	server_running = 0;
	pthread_join(server_thread_id, NULL);
	// Remove shared memory
	shmctl(shmid, IPC_RMID, NULL);
	exit(EXIT_SUCCESS);
}
