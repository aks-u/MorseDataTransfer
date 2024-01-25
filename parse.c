#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "logger.h"


int parse(int argc, char **argv, int *ifd, int *ofd) {
    if (argc == 2) { // Only input file, output stdout
        *ofd = STDOUT_FILENO;
        if (strcmp(argv[1], "-") == 0) {
            *ifd = STDIN_FILENO;
        } else {
            *ifd = open(argv[1], O_RDONLY);
            if (*ifd < 0) {
                fprintf(stderr, "Opening input file (%s) failed\n", argv[1]);
                logger("Opening input file failed");
                return -1;
            }
        }
    } else if (argc == 3) { // Both input and output file given
        if (strcmp(argv[1], "-") == 0) {
            *ifd = STDIN_FILENO;
        } else {
            *ifd = open(argv[1], O_RDONLY);
            if (*ifd < 0) {
                fprintf(stderr, "Opening input file (%s) failed\n", argv[1]);
                logger("Opening input file failed");
                return -1;
            }
        }
        if (strcmp(argv[2], "-") == 0) {
            *ofd = STDOUT_FILENO;
        } else {
            *ofd = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC, 0644);
            if (*ofd < 0) {
                fprintf(stderr,"Creating output file (%s) failed\n",argv[2]);
                logger("Creating output file failed");
                return -1;
            }
        }
    } else { // Incorrect usage
        fprintf(stderr,"Usage: %s [input|-] [output|-]\n",argv[0]);
        logger("Incorrect usage");
        return -1;
    }

    return 0;
}
