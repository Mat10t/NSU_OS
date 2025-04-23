#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
 
void createDirectory(const char *path) {
    if (mkdir(path, 0755) == 0) {
        printf("Folder %s created.\n", path);
    } else {
        perror("Folder creation error");
    }
}
 
void listDirectory(const char *path) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror("Folder opening error");
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }
    closedir(dir);
}
 
void removeDirectory(const char *path) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror("Folder opening error");
        return;
    }
    char tmpPath[1024];
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        snprintf(tmpPath, sizeof(tmpPath), "%s/%s", path, entry->d_name);
        struct stat fileStat;
        if(lstat(tmpPath, &fileStat) == -1) {
            perror("Error getting information about the file");
            continue;
        }
        if(S_ISDIR(fileStat.st_mode)) {
            removeDirectory(tmpPath);
        } else {
            unlink(tmpPath);
        }
    }
    closedir(dir);
    if (rmdir(path) == 0) {
        printf("Folder %s deleted.\n", path);
    } else {
        perror("Folder deletion error");
    }
}
 
void createFile(const char *path) {
    int file = open(path, O_CREAT | O_TRUNC);
    if (file == -1) {
        perror("Error creating the target file");
        return;
    }
    printf("File %s created.\n", path);
    close(file);
}
 
void readFile(const char *path) {
    int file = open(path, O_RDONLY);
    if (file == -1) {
        perror("File opening error");
        return;
    }

    char buffer[1024];
    ssize_t bytesRead;
 
    while ((bytesRead = read(file, buffer, sizeof(buffer))) > 0) {
        write(STDOUT_FILENO, buffer, bytesRead);
    }

    if (bytesRead == -1) {
        perror("File reading error");
    }
    close(file);
}
 
void removeFile(const char *path) {
    if (unlink(path) == 0) {
        printf("File %s deleted.\n", path);
    } else {
        perror("File deletion error");
    }
}
 
void createSymlink(const char *target, const char *linkpath) {
    if (symlink(target, linkpath) == 0) {
        printf("Symbolic link %s -> %s created.\n", linkpath, target);
    } else {
        perror("Error creating a symbolic link");
    }
}
 
void readSymlink(const char *path) {
    char buffer[512];
    ssize_t len = readlink(path, buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
        printf("%s -> %s\n", path, buffer);
    } else {
        perror("Error reading symbolic link");
    }
}
 
void removeSymlink(const char *path) {
    if (unlink(path) == 0) {
        printf("Symbolic link %s deleted.\n", path);
    } else {
        perror("Error deleting symbolic link");
    }
}
 
void createHardlink(const char *target, const char *linkpath) {
    if (link(target, linkpath) == 0) {
        printf("Hard link %s -> %s created.\n", linkpath, target);
    } else {
        perror("Error creating a hard link");
    }
}
 
void removeHardlink(const char *path) {
    if (unlink(path) == 0) {
        printf("Hard link %s removed.\n", path);
    } else {
        perror("Error deleting a hard link");
    }
}
 
void getFileInfo(const char *path) {
    struct stat file_stat;
    if (stat(path, &file_stat) == 0) {
        printf("Permissions: %o\n", file_stat.st_mode & 0777);
        printf("Number of hard links: %ld\n", file_stat.st_nlink);
    } else {
        perror("Error getting information about the file");
    }
}
 
void changePermissions(const char *path, mode_t mode) {
    if (chmod(path, mode) == 0) {
        printf("Permissions to %s changed to %o.\n", path, mode);
    } else {
        perror("Error changing permissions");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <path>\n", argv[0]);
        return EXIT_FAILURE;
    }
 
    char *progName = strrchr(argv[0], '/');
    progName = (progName) ? progName + 1 : argv[0];
 
    if (strcmp(progName, "createDir") == 0) {
        createDirectory(argv[1]);
    } else if (strcmp(progName, "listDir") == 0) {
        listDirectory(argv[1]);
    } else if (strcmp(progName, "removeDir") == 0) {
        removeDirectory(argv[1]);
    } else if (strcmp(progName, "createFile") == 0) {
        createFile(argv[1]);
    } else if (strcmp(progName, "readFile") == 0) {
        readFile(argv[1]);
    } else if (strcmp(progName, "removeFile") == 0) {
        removeFile(argv[1]);
    } else if (strcmp(progName, "createSymlink") == 0 && argc == 3) {
        createSymlink(argv[1], argv[2]);
    } else if (strcmp(progName, "readSymlink") == 0) {
        readSymlink(argv[1]);
    } else if (strcmp(progName, "removeSymlink") == 0) {
        removeSymlink(argv[1]);
    } else if (strcmp(progName, "createHardlink") == 0 && argc == 3) {
        createHardlink(argv[1], argv[2]);
    } else if (strcmp(progName, "removeHardlink") == 0) {
        removeHardlink(argv[1]);
    } else if (strcmp(progName, "fileInfo") == 0) {
        getFileInfo(argv[1]);
    } else if (strcmp(progName, "changePermissions") == 0 && argc == 3) {
        changePermissions(argv[1], strtol(argv[2], NULL, 8));
    } else {
        perror("Unknown command or incorrect number of arguments");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
