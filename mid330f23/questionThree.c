/*
Written by: Slava Borysyuk
Course: CS330
10/19/2023
Midterm Problem 3.1 and 3.2
Output:
input.txt
questionOne.c
questionThree.c
a.out
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

// Function to get the list of regular files in a directory
char **getRegularFiles(const char *nameOfDirectory) {
    DIR *dir = opendir(nameOfDirectory);
    if (!dir) {
        perror("Error opening directory");
        return NULL;
    }

    char **fileList = NULL;
    int fileCount = 0;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        char *entryPath = malloc(strlen(nameOfDirectory) + strlen(entry->d_name) + 2); 
        if (entryPath == NULL) {
            perror("Memory allocation error");
            return NULL;
        }
        sprintf(entryPath, "%s/%s", nameOfDirectory, entry->d_name);

        struct stat fileStat;
        if (stat(entryPath, &fileStat) < 0) {
            perror("Error getting file information");
            free(entryPath);
            continue;
        }

        if (S_ISREG(fileStat.st_mode)) {
            // Allocate memory for the new file name and copy it
            char *fileName = malloc(strlen(entry->d_name) + 1); 
            if (fileName == NULL) {
                perror("Memory allocation error");
                free(entryPath);
                continue;
            }
            strcpy(fileName, entry->d_name);

            // Resize the file list array and add the new file name
            fileList = realloc(fileList, (fileCount + 1) * sizeof(char *));
            if (fileList == NULL) {
                perror("Memory allocation error");
                free(entryPath);
                free(fileName);
                continue;
            }
            fileList[fileCount] = fileName;
            fileCount++;

            free(entryPath); 
        } else {
            free(entryPath); 
        }
    }

    closedir(dir);

    // Add a NULL pointer to the end of the list to signal the end
    fileList = realloc(fileList, (fileCount + 1) * sizeof(char *));
    if (fileList == NULL) {
        perror("Memory allocation error");
        return NULL;
    }
    fileList[fileCount] = NULL;

    return fileList;
}

int main() {
    const char *directoryName = "/home/borysyy/www/mid330f23";
    char **files = getRegularFiles(directoryName);
    if (files) {
        for (int i = 0; files[i] != NULL; i++) {
            printf("%s\n", files[i]);
            free(files[i]); 
        }
        free(files); 
    }
    return 0;
}
