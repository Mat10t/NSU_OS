#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int globalVar = 12345;

int main() {
    int localVar = 56234;
    printf("PID: %d\n", getpid());

    pid_t pid = fork();
    if(pid == -1) {
        perror("Fork error");
        return EXIT_FAILURE;
    } else if (pid == 0) {
        printf("Child PID: %d\n", getpid());
        exit(5);
    } else {
        sleep(60);
        printf("Parent exit\n");
    }

    return EXIT_SUCCESS;
}
