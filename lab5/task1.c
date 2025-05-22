#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int globalVar = 12345;

int main() {
    int localVar = 56234;
    printf("Global: %p - %d\n", &globalVar, globalVar);
    printf("Local: %p - %d\n", &localVar, localVar);
    printf("PID: %d\n", getpid());
    pid_t pid = fork();
    if(pid == -1) {
        perror("Fork error");
        return EXIT_FAILURE;
    } else if (pid == 0) {
        printf("Child PID: %d\n", getpid());
        printf("Parent PID: %d\n", getppid());
        printf("Child global: %p - %d\n", &globalVar, globalVar);
        printf("Child local: %p - %d\n", &localVar, localVar);  
        globalVar = 1;
        localVar = 2;
        printf("Child modified global: %p - %d\n", &globalVar, globalVar);
        printf("Child modified local: %p - %d\n", &localVar, localVar);
        exit(5);
    } else {
        printf("Parent global: %p - %d\n", &globalVar, globalVar);
        printf("Parent local: %p - %d\n", &localVar, localVar);
        sleep(30);
        int status;
        pid_t chPid = wait(&status);
        if (chPid == -1) {
            perror("Wait error");
            return 1;
        }
        if (WIFEXITED(status)) {
            printf("Exit code: %d\n", WEXITSTATUS(status));
        } else {
            printf("Unexpected status\n");
        }
    }
    return EXIT_SUCCESS;
}
