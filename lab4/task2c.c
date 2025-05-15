#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/mman.h>

#define HEAP_ALLOCATIONS 100000
#define PAGE_COUNT 10

void sigsegvHandler(int sig) {
    printf("Caught signal %d (SIGSEGV)\n", sig);
    exit(EXIT_FAILURE);
}

void stackGrowth(int depth) {
    char buffer[4096];
    printf("Stack %d kb\n", depth);
    sleep(1);
    stackGrowth(depth + 1);
}

void heapGrowth() {
    char *blocks[HEAP_ALLOCATIONS];
    for(int i = 0; i < HEAP_ALLOCATIONS; i++) {
        blocks[i] = NULL;
    }
    for (int i = 0; i < HEAP_ALLOCATIONS; i++) {
        blocks[i] = malloc(sizeof(*blocks[i]) * 1024 * 300);
        if (blocks[i] == NULL) {
            perror("Error malloc");
            for (int j = 0; j < i; j++) {
                free(blocks[j]);
            }
            return;
        }
        sleep(1);
    }

    for (int i = 0; i < HEAP_ALLOCATIONS; i++) {
        free(blocks[i]);
    }
}

char* allocateMmap() {
    size_t pagesize = sysconf(_SC_PAGESIZE);
    char* addr = mmap(NULL, pagesize * 10, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED) {
        perror("Error allocate mmap");
        return NULL;
    }

    printf("mmap region: %p\n", addr);
    return addr;
}

void testReadProtection(char* addr) {
    if(!addr) {
        return;
    }
    size_t pagesize = sysconf(_SC_PAGESIZE);

    strcpy(addr, "ABCD\0");
    printf("Value: %s\n", addr);

    if (mprotect(addr, pagesize * PAGE_COUNT, PROT_NONE) != 0) {
        perror("Error mprotect");
    }
    printf("Attempting to read from protected region...\n");
    printf("Value: %s\n", addr);
}

void testWriteProtection(char* addr) {
    if(!addr) {
        return;
    }
    size_t pagesize = sysconf(_SC_PAGESIZE);

    strcpy(addr, "ABCD\0");
    printf("Value: %s\n", addr);

    if (mprotect(addr, pagesize * PAGE_COUNT, PROT_NONE) != 0) {
        perror("Error mprotect");
    }
    printf("Attempting to write to protected region...\n");
    addr[0] = 'E';
}

void unmapMiddle(char* addr) {
    size_t pagesize = sysconf(_SC_PAGESIZE);
    if (munmap(addr + 4 * pagesize, 2 * pagesize) != 0) {
        perror("Error munmap");
    } else {
        printf("Unmapped pages 4 to 6\n");
    }
}

int main(int argc, char* argv[]) {
    printf("PID: %d\n", getpid());
    signal(SIGSEGV, sigsegvHandler);
    sleep(10);

    //stackGrowth(0);

    //heapGrowth();

    char* mmapRegion = allocateMmap();

    //testReadProtection(mmapRegion);
    //testWriteProtection(mmapRegion);

    unmapMiddle(mmapRegion);
    getchar();
    return 0;
}
