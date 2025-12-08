#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command> [args...]\n", argv[0]);
        return 1;
    }
    
    // Set real and effective UID/GID to root
    if (setuid(0) != 0) {
        perror("setuid failed");
        return 1;
    }
    
    if (setgid(0) != 0) {
        perror("setgid failed");
        return 1;
    }
    
    // Execute the command with all remaining arguments
    execvp(argv[1], &argv[1]);
    
    // If execvp returns, it failed
    perror("execvp failed");
    return 1;
}
