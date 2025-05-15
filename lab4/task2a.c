#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    printf("PID: %d\n", getpid());
    sleep(1);

    execl(argv[0], argv[0], NULL);
    perror("execl failed");
    printf("Hello world!\n");

    // if (argc == 1) {
    //     execl(argv[0], argv[0], "second", NULL);
    //     perror("execl failed");
    // } else {
    //     printf("Hello world!\n");
    // }

    return 0;
}
