/*

Written by: Slava Borysyuk
Course: CS330
10/6/2023
HW5_Threads
Output:

Input File: input.txt
Search Character: a
Number of Threads: 1
Occurrences: 38554
Elapsed Time: 0 seconds and 9978 microseconds

Input File: input.txt
Search Character: a
Number of Threads: 2
Occurrences: 38554
Elapsed Time: 0 seconds and 5592 microseconds

Input File: input.txt
Search Character: a
Number of Threads: 3
Occurrences: 38554
Elapsed Time: 0 seconds and 2845 microseconds

Input File: input.txt
Search Character: a
Number of Threads: 4
Occurrences: 38554
Elapsed Time: 0 seconds and 3055 microseconds

*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define MAX_BUFFER_SIZE 1000000 // Maximum size for the input file
#define MAX_FILE_NAME_SIZE 256 // Maximum size for the file name

struct timeval start_time, end_time;

struct ThreadData {
    char* buffer;           // Pointer to the input buffer
    char search_char;       // Character to search for
    int start_index;        // Start index for this thread's portion of the buffer
    int end_index;          // End index for this thread's portion of the buffer
    int count;              // Count of occurrences found by this thread
};

// Function to start the timer
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

// Function for each thread to count occurrences of 'search_char' in its portion of the buffer
void* count_occurrences(void* arg) {
    struct ThreadData* data = (struct ThreadData*)arg;
    int count = 0;

    for (int i = data->start_index; i <= data->end_index; i++) {
        if (data->buffer[i] == data->search_char) {
            count++;
        }
    }
    data->count = count;
    return NULL;
}

int main(void) {
    char input_file[MAX_FILE_NAME_SIZE]; // Store the input file name
    char search_char;
    int num_threads = 0;

    printf("Enter the input file: ");
    scanf("%s", input_file);

    printf("Enter a character to search: ");
    scanf(" %c", &search_char);

    printf("Enter the number of threads: ");
    scanf("%d", &num_threads);

    FILE* file = fopen(input_file, "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    char buffer[MAX_BUFFER_SIZE]; // Buffer to hold the file content
    size_t file_size = fread(buffer, 1, MAX_BUFFER_SIZE, file);
    fclose(file);

    pthread_t threads[num_threads];
    struct ThreadData thread_data[num_threads];

    starttimer();

    // Create and launch threads
    for (int i = 0; i < num_threads; i++) {
        int start_index = i * (file_size / num_threads);
        int end_index = (i == num_threads - 1) ? file_size - 1 : (i + 1) * (file_size / num_threads) - 1;

        thread_data[i].buffer = buffer;
        thread_data[i].search_char = search_char;
        thread_data[i].start_index = start_index;
        thread_data[i].end_index = end_index;
        pthread_create(&threads[i], NULL, count_occurrences, &thread_data[i]);
    }

    // Wait for threads to finish
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    int total_count = 0;
    for (int i = 0; i < num_threads; i++) {
        total_count += thread_data[i].count;
    }

    endtimer();

    printf("\n");
    printf("Input File: %s\n", input_file);
    printf("Search Character: %c\n", search_char);
    printf("Number of Threads: %d\n", num_threads);
    printf("Occurrences: %d\n", total_count);
    printtime();

    return 0;
}

