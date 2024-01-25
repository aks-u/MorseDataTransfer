#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include "logger.h"

// External constants and variables from main.c
extern const char alpha[];
extern const char *morse[];
extern sem_t *sem;

// Child process function
void child(pid_t ppid, int ifd) {
    logger("Child process active");

    char buf;
    while (read(ifd, &buf, 1) > 0) {
        // Find the index of the character 'buf' in the 'alpha' array. Use it to find the correct Morse code representation
        char *res = strchr(alpha, buf);
        int index = (int)(res - alpha);
        
        // Iterate over the Morse code representation of the character
        for (int j = 0; j < strlen(morse[index]); j++) {
                if (morse[index][j] == '.') {
                    kill(ppid, SIGUSR1);        // Signal the parent process with SIGUSR1 for a dot in Morse code
                }
                else if (morse[index][j] == '-') {
                    kill(ppid, SIGUSR2);        // Signal the parent process with SIGUSR2 for a dash in Morse code
                }
                sem_wait(sem);      // Wait for a semaphore, for synchronization
        }
        kill(ppid, SIGINT);     // Signal the parent process with SIGINT, indicating the end of a character
        sem_wait(sem);          // Wait for a semaphore, for synchronization
    }

    close(ifd);     // Close the input file descriptor   
    kill(ppid, SIGINT);     // Signal the parent process with a consecutive SIGINT to signal the end of the input file
    logger("Child process finished");
    exit(0);    // Exit the child process
}

