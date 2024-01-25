#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include "logger.h"
#include "parse.h"
#include "child.h"
#include "parent.h"


static int sigpipe = 0;
int pipefd[2];
sem_t *sem;


// Morse code characters and their corresponding representations
const char alpha[] ="ABCDEFGHIJLKMNOPQRSTUVWXYZ1234567890&'@():,=!.-+\"?/";
const char *morse[] = {
    // Letters: ABCDEFGHIJLKMNOPQRSTUVWXYZ
    ".-",   "-...", "-.-.", "-..",  ".",    "..-.", "--.",   "....", "..",   ".---", 
    "-.-",  ".-..", "--",   "-.",   "---",  ".--.", "--.-",  ".-.",  "...",  "-", 
    "..-",  "...-", ".--",  "-..-", "-.--", "--..", 
    // Numbers: 1234567890
    ".----", "..---", "...--", "....-", ".....", "-....", "--...", "---..", "----.", "-----",
    // Punctuation marks: &'@():,=!.-+"?/
    ".-...", ".----.", ".--.-.", "-.--.", "-.--.-", "---...", "--..--", "-...-", "-.-.--",
    ".-.-.-", "-....-", ".-.-.", ".-..-.", "..--..", "-..-."
};


// Signal handler
static void handler(int sig) {
	char ret = sig;
	write(sigpipe, &ret, 1);        // Write the sent signal to the pipe
    sem_post(sem);      // Semaphore for inter-process synchronization
}


int main(int argc, char **argv) {
    logger("Program starts");

    // Semaphore for inter-process synchronization
    sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_init(sem, 1, 0);

    int ifd, ofd;

    // Parse command-line arguments to obtain input and output file descriptors
    if (parse(argc, argv, &ifd, &ofd) == -1) {
        return -1;
    } logger("Command line parsing successful");
	
    // Create a pipe for inter-process communication
	assert(pipe(pipefd) == 0);
	sigpipe = pipefd[1];

    // Set up the signal handler
    struct sigaction sig;
	sigemptyset(&sig.sa_mask);
  	sig.sa_flags = SA_RESTART;
	sig.sa_handler = handler;

    // Register signal handler for SIGUSR1, SIGUSR2, and SIGINT
	assert((sigaction(SIGUSR1,  &sig, NULL)) == 0); 
	assert((sigaction(SIGUSR2,  &sig, NULL)) == 0);
    assert((sigaction(SIGINT,   &sig, NULL)) == 0);
    logger("Handler set for signals SIGUSR1, SIGUSR2, SIGINT");

    // Create a child process
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        logger("Fork failed");
        exit(1);
    }

    if (pid == 0) {
        // Child process
        pid_t ppid = getppid();
        child(ppid, ifd);
    }
    else {
        // Parent process
        parent(ofd);
    }
	return 0;
}
