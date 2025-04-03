#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 4096

void reverseString(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}

void copyAndReverseFile(const char *srcPath, const char *dstPath) {
    int src = open(srcPath, O_RDONLY);
    if (src == -1) {
        perror("Error opening the source file");
        return;
    }

    int dst = open(dstPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst == -1) {
        perror("Error creating the target file");
        close(src);
        return;
    }

    off_t offset = lseek(src, 0, SEEK_END);
    if (offset == -1) {
        perror("File size detection error");
        close(src);
        close(dst);
        return;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;

    while (offset > 0) {
        size_t chunkSize = (offset >= BUFFER_SIZE) ? BUFFER_SIZE : offset;
        offset -= chunkSize;

        if (lseek(src, offset, SEEK_SET) == -1) {
            perror("Error during lseek");
            break;
        }

        bytesRead = read(src, buffer, chunkSize);
        if (bytesRead <= 0) {
            perror("File reading error");
            break;
        }

        for (size_t i = 0; i < bytesRead / 2; i++) {
            char temp = buffer[i];
            buffer[i] = buffer[bytesRead - i - 1];
            buffer[bytesRead - i - 1] = temp;
        }

        if (write(dst, buffer, bytesRead) != bytesRead) {
            perror("File writing error");
            break;
        }
    }

    close(src);
    close(dst);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *srcDir = argv[1];
    struct stat statbuf;
    if (stat(srcDir, &statbuf) != 0 || !S_ISDIR(statbuf.st_mode)) {
        fprintf(stderr, "Error: %s It is not a catalog\n", srcDir);
        return EXIT_FAILURE;
    }

    char reversedDir[256];
    strncpy(reversedDir, srcDir, sizeof(reversedDir) - 1);
    reversedDir[sizeof(reversedDir) - 1] = '\0';
    reverseString(reversedDir);

    if (mkdir(reversedDir, 0755) != 0) {
        perror("Folder creation error");
        return EXIT_FAILURE;
    }

    DIR *dir = opendir(srcDir);
    if (!dir) {
        perror("Folder opening error");
        return EXIT_FAILURE;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char srcPath[512];
        snprintf(srcPath, sizeof(srcPath), "%s/%s", srcDir, entry->d_name);

        struct stat fileStat;
        if (stat(srcPath, &fileStat) != 0 || !S_ISREG(fileStat.st_mode))
            continue;

        char reversedFilename[256];
        strncpy(reversedFilename, entry->d_name, sizeof(reversedFilename) - 1);
        reversedFilename[sizeof(reversedFilename) - 1] = '\0';
        reverseString(reversedFilename);

        char dstPath[512];
        snprintf(dstPath, sizeof(dstPath), "%s/%s", reversedDir, reversedFilename);

        copyAndReverseFile(srcPath, dstPath);
    }

    closedir(dir);
    return EXIT_SUCCESS;
}
