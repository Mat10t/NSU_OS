#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
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

mode_t setPermission(const char *path) {
    struct stat statBuf;
    if (stat(path, &statBuf) == -1) {
        perror("Error permisson reading");
        return 0;
    }
    mode_t originalPermission = statBuf.st_mode & 0777;
    if(chmod(path, 0777) == -1) {
        perror("Permission change error");
    } 
    return originalPermission;
}

mode_t getPermission(const char *path) {
    struct stat statBuf;
    if (stat(path, &statBuf) == -1) {
        perror("Error permisson reading");
        return 0;
    }
    return statBuf.st_mode & 0777;
}

void copyAndReverseFile(const char *srcPath, const char *dstPath, mode_t newPermission) {
    int src = open(srcPath, O_RDONLY);
    if (src == -1) {
        perror("Error opening the source file");
        return;
    }

    mode_t oldPermission;
    int permissionFlag = 0;
    struct stat statBuf;
    if (stat(dstPath, &statBuf) != -1) {
        oldPermission = setPermission(dstPath);
        permissionFlag = 1; 
    }

    int dst = open(dstPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst == -1) {
        perror("Error creating the target file");
        if (permissionFlag) {
            if (chmod(dstPath, oldPermission) == -1) {
                perror("Permission change error");
            } 
        }
        if(close(src) == -1) {
            perror("File closing error");
        }
        return;
    }

    off_t offset = lseek(src, 0, SEEK_END);
    if (offset == -1) {
        perror("File size detection error");
        if (permissionFlag) {
            if (chmod(dstPath, oldPermission) == -1) {
                perror("Permission change error");
            }
        }
        if(close(src) == -1) {
            perror("File closing error");
        }
        if(close(dst) == -1) {
            perror("File closing error");
        }
        return;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;

    while (offset > 0) {
        size_t chunkSize = (offset >= BUFFER_SIZE) ? BUFFER_SIZE : offset;
        offset -= chunkSize;

        if (lseek(src, offset, SEEK_SET) == -1) {
            perror("Error during lseek");
            if (permissionFlag) {
                if (chmod(dstPath, oldPermission) == -1) {
                    perror("Permission change error");
                }
            }
            if(close(src) == -1) {
                perror("File closing error");
            }
            if(close(dst) == -1) {
                perror("File closing error");
            }
            break;
        }

        bytesRead = read(src, buffer, chunkSize);
        if (bytesRead <= 0) {
            perror("File reading error");
            if (permissionFlag) {
                if (chmod(dstPath, oldPermission) == -1) {
                    perror("Permission change error");
                }
            }
            if(close(src) == -1) {
                perror("File closing error");
            }
            if(close(dst) == -1) {
                perror("File closing error");
            }
            return;
        }

        for (size_t i = 0; i < bytesRead / 2; i++) {
            char temp = buffer[i];
            buffer[i] = buffer[bytesRead - i - 1];
            buffer[bytesRead - i - 1] = temp;
        }

        ssize_t bytesWrite = write(dst, buffer, bytesRead);
        if(bytesWrite != bytesRead) {
            write(dst, buffer + bytesWrite, bytesRead - bytesWrite);
        }
    }

    if(fchmod(dst, newPermission) == -1) {
        perror("Permission change error");
    }
    if(close(src) == -1) {
        perror("File closing error");
    }
    if(close(dst) == -1) {
        perror("File closing error");
    }
}

void reverseDir(const char *srcDir, const char *dstDir) {

    struct stat dirStat;
    if (stat(srcDir, &dirStat) == -1) {
        perror("Error directory permisson reading");
        return;
    }

    mode_t oldPermissionsSrc = getPermission(srcDir);
    mode_t oldPermissionDst;
    int permissionFlag = 0;
    //sleep(10000);

    DIR *dir = opendir(srcDir);
    if (!dir) {
        perror("Folder opening error");
        if (permissionFlag) {
            if (chmod(dstDir, oldPermissionDst) == -1) {
                perror("Permission change error");
            }
        }
        return;
    }

    if (stat(dstDir, &dirStat) == -1) {
        if (mkdir(dstDir, 0755) != 0) {
            perror("Folder creation error");
            return;
        }
    } else {
        oldPermissionDst = setPermission(dstDir);
        permissionFlag = 1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char srcPath[BUFFER_SIZE];
        snprintf(srcPath, sizeof(srcPath), "%s/%s", srcDir, entry->d_name);

        char reversedDir[1024];
        strncpy(reversedDir, entry->d_name, sizeof(reversedDir) - 1);
        reverseString(reversedDir);
        char dstPath[BUFFER_SIZE];
        snprintf(dstPath, sizeof(dstPath), "%s/%s", dstDir, reversedDir);

        if (entry->d_type == DT_REG) {
            mode_t fileMode = getPermission(srcPath);
            copyAndReverseFile(srcPath, dstPath, fileMode);
            if (chmod(srcPath, fileMode) == -1) {
                perror("Permission change error");
            }
        } else if (entry->d_type == DT_DIR) {
            reverseDir(srcPath, dstPath);
        }
    }

    if(closedir(dir) == -1) {
        perror("Directory closing error");
    }
    if (chmod(srcDir, oldPermissionsSrc) == -1) {
        perror("Permission change error");
    }
    if (chmod(dstDir, oldPermissionsSrc) == -1) {
        perror("Permission change error");
    }
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

    char reversedDir[BUFFER_SIZE];
    strncpy(reversedDir, srcDir, sizeof(reversedDir) - 1);
    char *name = strrchr(reversedDir, '/');
    name = (name != NULL) ? name + 1 : reversedDir;
    reverseString(name);

    reverseDir(srcDir, reversedDir);

    return EXIT_SUCCESS;
}
