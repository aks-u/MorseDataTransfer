#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include "logger.h"

// External constants and variables from main.c
extern const char alpha[];
extern const char *morse[];
extern int pipefd[2];

// Function to define the behavior of the parent process
void parent(int ofd) {
    logger("Parent process active");

    char buffer[6+1];       // Create a buffer to store Morse code characters and a null terminator
    memset(buffer, 0, sizeof(buffer));      // Initialize the buffer with null characters
    
    int i = 0;
    for ( ; ; ) {
        char Rxsignal;
        int res = read(pipefd[0],&Rxsignal,1);      // Read the pipe for the received signals

        if (res < 0) {
            perror("Reading pipe failed");
            logger("Reading pipe failed");
        }
        if (res == 1) {
            if (Rxsignal == SIGUSR1) {
                buffer[i] = '.';        // Append a dot to the buffer
                i++;
            }        
            if (Rxsignal == SIGUSR2) {
                buffer[i] = '-';        // Append a dash to the buffer
                i++;
            }        
            if (Rxsignal == SIGINT) {
                if (strlen(buffer) == 0) {  // If the buffer is empty when SIGINT has been received (two consecutive SIGINT signals), exit the parent process
                    logger("Parent process/Program finished");
                    logger("-------------------------------------------");
                    exit(0);
                }

                int index;
                // Find the Morse code representation in the 'morse' array
                for (int j = 0; j < strlen(alpha); j++) {
                    if (strcmp(buffer, morse[j]) == 0) {
                        index = j;
                        break;
                    }   
                }

                char character = alpha[index];
                // Write the decoded character to the output file descriptor
                write(ofd, &character, 1);
                memset(buffer, 0, sizeof(buffer));      // Clear the buffer
                i = 0;      // Reset the index
            }
        }
    }
}