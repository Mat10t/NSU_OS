#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int globalInited = 1;
int globalUninited;
const int globalConst = 2;

// int* returnLocalAdress() {
//     int localVar = 0;
//     printf("Address of localVar: %p\n", &localVar);
//     return &localVar; //не компилируется
// }

void printAndModifyEnv() {
    const char* name = "MY_ENV_VAR";

    char* value = getenv(name);
    printf("Environment variable value: %s\n", value);

    setenv(name, "Evil Hello!", 1);

    value = getenv(name);
    printf("New environment variable value: %s\n", value);
}

void printDynamicMemory() {
    char *buffer = malloc(100 * sizeof(*buffer));
    if (buffer == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }

    strcpy(buffer, "hello world");
    printf("Buffer: %s\n", buffer);
    free(buffer);

    printf("Buffeer after free: %s\n", buffer);

    buffer = (char*)malloc(100 * sizeof(*buffer));
    if (buffer == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }

    strcpy(buffer, "new hello world");
    printf("New buffer content: %s\n", buffer);
    
    char *midBuffer = buffer + 50;

    //free(midBuffer); //не компилируется

    //printf("After freeing midBuffer, buffer content: %s\n", buffer);

    free(buffer);
}

void printAddresses() {
    int localVarInited = 3;
    static int staticVarInited = 4;
    const int constVarInited = 5;

    int localVarUninited;
    static int staticVarUninited;
    const int constVarUninited;

    printf("Local inited:\n");
    printf("Local variable address: %p\n", &localVarInited);
    printf("Static variable address: %p\n", &staticVarInited);
    printf("Const variable address: %p\n", &constVarInited);

    printf("Local uninited:\n");
    printf("Local variable address: %p\n", &localVarUninited);
    printf("Static variable address: %p\n", &staticVarUninited);
    printf("Const variable address: %p\n", &constVarUninited);

    printf("Global:\n");
    printf("Global initialized address: %p\n", &globalInited);
    printf("Global uninitialized address: %p\n", &globalUninited);
    printf("Global const address: %p\n", &globalConst);
}

int main() {
    printf("a\n");
    printAddresses();
    // printf("d\n");
    // int* ptr = returnLocalAdress();
    // printf("Address received in main: %p\n", ptr);
    printf("e\n");
    printDynamicMemory();
    printf("h\n");
    printAndModifyEnv();
    printf("PID: %d\n", getpid());
    getchar();
    return 0;
}
