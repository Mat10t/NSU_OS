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

        pid_t nextPid = fork();
        if(nextPid == -1) {
            perror("Fork error");
            return EXIT_FAILURE;
        } else if (nextPid == 0) {
            printf("Child's Child PID: %d\n", getpid());
            sleep(60);
            exit(0);
        } else {
            exit(0);
        }
        exit(5);
    } else {
        sleep(100);
        printf("Parent exit\n");
    }

    return EXIT_SUCCESS;
}
