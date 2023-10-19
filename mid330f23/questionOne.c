/*
Written by: Slava Borysyuk
Course: CS330
10/19/2023
Midterm Problem 1.1, 1.2, 1.3, 1.4
Output:
Count was: 31
Elapsed Time: 0 seconds and 1685 microseconds
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>

#define MAX_ARRAY_SIZE 1000 // Maximum size for the input array

// Problem 1.2
typedef struct {
    int occurrences;
} TimeCountResult;

// Function to start the timer from HW2
struct timeval start_time, end_time;
void starttimer() {
    gettimeofday(&start_time, NULL);
}

// Function to stop the timer
void endtimer() {
    gettimeofday(&end_time, NULL);
}

// Function to print the elapsed time
void printtime() {
    long start_MS = start_time.tv_sec * 1000000 + start_time.tv_usec;
    long end_MS = end_time.tv_sec * 1000000 + end_time.tv_usec;
    long totalelapsed = end_MS - start_MS;

    long seconds = totalelapsed / 1000000;
    long microseconds = totalelapsed % 1000000;

    printf("Elapsed Time: %ld seconds and %ld microseconds\n", seconds, microseconds);
}

// Modified count function taking only numforkedprocesses and searchchar
int count(int numforkedprocesses, char searchchar, char *input_array, int array_size) {
    int total_count = 0;

    // Calculate the chunk size for each process
    int chunk_size = array_size / numforkedprocesses;

    for (int i = 0; i < numforkedprocesses; i++) {
        int start = i * chunk_size;
        int end = (i == numforkedprocesses - 1) ? array_size : (i + 1) * chunk_size;
        pid_t pid = fork();

        if (pid == 0) {
            // Child process
            int local_count = 0;
            for (int j = start; j < end; j++) {
                if (input_array[j] == searchchar) {
                    local_count++;
                }
            }
            exit(local_count);
        } else if (pid < 0) {
            perror("Fork failed");
            exit(1);
        }
    }

    // Parent process: wait for child processes to finish and collect their results
    for (int i = 0; i < numforkedprocesses; i++) {
        int status;
        wait(&status);

        if (WIFEXITED(status)) {
            total_count += WEXITSTATUS(status);
        }
    }

    return total_count;
}

// Problem 1.2
TimeCountResult timedCount(int numforkedprocesses, char searchchar, char *input_array, int array_size) {
    TimeCountResult result;
    starttimer();

    result.occurrences = count(numforkedprocesses, searchchar, input_array, array_size);

    endtimer();

    return result;
}

// Problem 1.1
int main() {
    char searchchar = 'k';      // Hardcoded search character
    int numforkedprocesses = 4; // Hardcoded number of processes

    char input_array[MAX_ARRAY_SIZE];
    int array_size = 0;

    // Read input from the file
    char filename[] = "input.txt"; // Change this to your input file name
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open the input file");
        return 1;
    }

    while ((array_size < MAX_ARRAY_SIZE) && (fscanf(file, "%c", &input_array[array_size]) == 1)) {
        array_size++;
    }

    fclose(file);

    // Problem 1.3
    TimeCountResult result = timedCount(numforkedprocesses, searchchar, input_array, array_size);

    printf("Count was: %d\n", result.occurrences);
    printtime();

    return 0;
}

// Problem 1.4
/*
Forking processes always involves some overhead, this includes
memory allocation and copying the process image. This can reduce
overall performance gain.

The OS must do context switching when managing multiple processes.
Constant context switching can lead to overhead.

Forking processes duplicated the memory space of the parent process.
If for example the array is large, the memory usage can become a problem.

Creating and terminating processes uses a lot of time and resources. If there
are too many processes, it can lead to serious resource usage.

In this program there could be problems with forking if the array size is too large,
which is why we maxed it out to 1000.
*/
