/*
Written by: Slava Borysyuk
Course: CS330
11/30/2023
HW11_PIPE
Output:
Producer 2498: 'work1' for 202074243 nanoseconds sent at 1701371264 seconds
Consumer 2499: 'work1' for 202074243 nanoseconds sent at 1701371264 seconds
Producer 2498: 'work2' for 232123518 nanoseconds sent at 1701371265 seconds
Consumer 2500: 'work2' for 232123518 nanoseconds sent at 1701371265 seconds
Producer 2498: 'work3' for 287183924 nanoseconds sent at 1701371265 seconds
Consumer 2501: 'work3' for 287183924 nanoseconds sent at 1701371265 seconds
Producer 2498: 'work4' for 329069990 nanoseconds sent at 1701371265 seconds
Producer 2498: 'work5' for 350539224 nanoseconds sent at 1701371266 seconds
Producer 2498: 'work6' for 199109302 nanoseconds sent at 1701371266 seconds
Consumer 2502: 'work4' for 329069990 nanoseconds sent at 1701371266 seconds
Consumer 2499: 'work6' for 199109302 nanoseconds sent at 1701371266 seconds
Consumer 2503: 'work5' for 350539224 nanoseconds sent at 1701371266 seconds
Producer 2498: 'work7' for 48451921 nanoseconds sent at 1701371266 seconds
Producer 2498: 'work8' for 269614985 nanoseconds sent at 1701371266 seconds
Consumer 2500: 'work7' for 48451921 nanoseconds sent at 1701371266 seconds
Producer 2498: 'work9' for 476370403 nanoseconds sent at 1701371266 seconds
Consumer 2501: 'work8' for 269614985 nanoseconds sent at 1701371266 seconds
Producer 2498: 'work10' for 277900370 nanoseconds sent at 1701371267 seconds
Producer 2498: 'work11' for 140768912 nanoseconds sent at 1701371267 seconds
Consumer 2503: 'work11' for 140768912 nanoseconds sent at 1701371267 seconds
Consumer 2502: 'work9' for 476370403 nanoseconds sent at 1701371267 seconds
Producer 2498: 'work12' for 195355231 nanoseconds sent at 1701371267 seconds
Consumer 2499: 'work10' for 277900370 nanoseconds sent at 1701371267 seconds
Consumer 2500: 'work12' for 195355231 nanoseconds sent at 1701371267 seconds
Producer 2498: 'work13' for 127819136 nanoseconds sent at 1701371267 seconds
Consumer 2501: 'work13' for 127819136 nanoseconds sent at 1701371267 seconds
Producer 2498: 'work14' for 427306102 nanoseconds sent at 1701371268 seconds
Producer 2498: 'work15' for 264312327 nanoseconds sent at 1701371268 seconds
Consumer 2502: 'work15' for 264312327 nanoseconds sent at 1701371268 seconds
Consumer 2503: 'work14' for 427306102 nanoseconds sent at 1701371268 seconds
Producer 2498: 'work16' for 33757414 nanoseconds sent at 1701371268 seconds
Consumer 2499: 'work16' for 33757414 nanoseconds sent at 1701371268 seconds
Producer 2498: 'work17' for 226198461 nanoseconds sent at 1701371269 seconds
Consumer 2500: 'work17' for 226198461 nanoseconds sent at 1701371269 seconds
Producer 2498: 'work18' for 325695638 nanoseconds sent at 1701371269 seconds
Producer 2498: 'work19' for 436988343 nanoseconds sent at 1701371269 seconds
Producer 2498: 'work20' for 138482593 nanoseconds sent at 1701371269 seconds
Consumer 2501: 'work18' for 325695638 nanoseconds sent at 1701371269 seconds
Consumer 2503: 'work20' for 138482593 nanoseconds sent at 1701371269 seconds
Consumer 2502: 'work19' for 436988343 nanoseconds sent at 1701371270 seconds
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <semaphore.h>
#include <fcntl.h>

#define MAX_WORK_DESCRIPTION_LENGTH 50
#define MAX_CONSUMERS 5

#define READ_END 0
#define WRITE_END 1

#define CONSUMER_READY "/consumer_ready"
#define PRODUCER_READY "/producer_ready"

sem_t *consumer_ready; // Semaphore for consumer synchronization
sem_t *producer_ready; // Semaphore for producer synchronization

struct msg_buffer
{
    char work_description[MAX_WORK_DESCRIPTION_LENGTH];
    long execution_time;
};

void consumer(int pipe_fd[])
{
    // Function to simulate consumer behavior

    char log_file_name[30];
    sprintf(log_file_name, "consumer_%d.log", getpid()); // Use process ID in log file name

    while (1)
    {
        char message_buffer[sizeof(struct msg_buffer)];

        // Wait until the producer signals that work is ready
        if (sem_wait(producer_ready) == -1)
        {
            perror("Error waiting for producer semaphore");
            exit(EXIT_FAILURE);
        }

        // Read message from the pipe
        if (read(pipe_fd[READ_END], message_buffer, sizeof(struct msg_buffer)) == -1)
        {
            perror("Error reading from pipe");
            exit(EXIT_FAILURE);
        }

        struct msg_buffer *message = (struct msg_buffer *)message_buffer;

        // Trim newline characters if present in work_description
        size_t length = strlen(message->work_description);
        if (message->work_description[length - 1] == '\n')
        {
            message->work_description[length - 1] = '\0';
        }

        // Simulate execution time
        struct timespec sleep_time, remaining_time;
        sleep_time.tv_sec = 0;
        sleep_time.tv_nsec = message->execution_time;
        nanosleep(&sleep_time, &remaining_time);

        // Print and log work details
        printf("%s %d: '%s' for %ld nanoseconds sent at %ld seconds\n",
               "Consumer", getpid(), message->work_description, message->execution_time, time(NULL));

        FILE *log_file = fopen(log_file_name, "a");
        if (!log_file)
        {
            perror("Error opening log file");
            exit(EXIT_FAILURE);
        }

        // Log work details to the file
        fprintf(log_file, "%s %d: '%s' for %ld nanoseconds sent at %ld seconds\n",
                "Consumer", getpid(), message->work_description, message->execution_time, time(NULL));

        fclose(log_file);

        // Signal producer that a consumer is ready for more work
        if (sem_post(consumer_ready) == -1)
        {
            perror("Error signaling consumer semaphore");
            exit(EXIT_FAILURE);
        }
    }
}

void producer(int pipe_fd[])
{
    // Function to simulate producer behavior

    srand(time(NULL));
    FILE *work_file = fopen("work.txt", "r");
    if (!work_file)
    {
        perror("Error opening work file");
        exit(EXIT_FAILURE);
    }

    pid_t consumer_pids[MAX_CONSUMERS];
    for (int i = 0; i < MAX_CONSUMERS; ++i)
    {
        consumer_pids[i] = fork();
        if (consumer_pids[i] == -1)
        {
            perror("Error forking consumer process");
            exit(EXIT_FAILURE);
        }
        else if (consumer_pids[i] == 0)
        {
            consumer(pipe_fd);
            exit(EXIT_SUCCESS);
        }
    }

    char line[MAX_WORK_DESCRIPTION_LENGTH];

    char log_file_name[30];
    sprintf(log_file_name, "producer_%d.log", getpid()); // Use process ID in log file name

    while (fgets(line, sizeof(line), work_file) != NULL)
    {
        // Wait until a consumer is ready to receive work
        if (sem_wait(consumer_ready) == -1)
        {
            perror("Error waiting for consumer semaphore");
            exit(EXIT_FAILURE);
        }

        struct msg_buffer message;

        // Read work description from file
        size_t length = strlen(line);
        if (line[length - 1] == '\n')
        {
            line[length - 1] = '\0'; // Removing the newline character
        }
        strncpy(message.work_description, line, sizeof(line));

        // Generate random execution time
        message.execution_time = rand() % 500000000;

        // Write message to the pipe
        if (write(pipe_fd[WRITE_END], &message, sizeof(struct msg_buffer)) == -1)
        {
            perror("Error writing to pipe");
            exit(EXIT_FAILURE);
        }

        // Simulate some time before producing the next work
        struct timespec sleep_time, remaining_time;
        sleep_time.tv_sec = 0;
        sleep_time.tv_nsec = rand() % 500000000;
        nanosleep(&sleep_time, &remaining_time);

        // Print and log work details
        printf("%s %d: '%s' for %ld nanoseconds sent at %ld seconds\n",
               "Producer", getpid(), message.work_description, message.execution_time, time(NULL));

        FILE *log_file = fopen(log_file_name, "a");
        if (!log_file)
        {
            perror("Error opening log file");
            exit(EXIT_FAILURE);
        }

        // Log work details to the file
        fprintf(log_file, "%s %d: '%s' for %ld nanoseconds sent at %ld seconds\n",
                "Producer", getpid(), message.work_description, message.execution_time, time(NULL));

        fclose(log_file);

        // Signal consumer that more work is available
        if (sem_post(producer_ready) == -1)
        {
            perror("Error signaling producer semaphore");
            exit(EXIT_FAILURE);
        }
    }

    fclose(work_file);

    // Wait for all child processes to complete
    for (int i = 0; i < MAX_CONSUMERS; ++i)
    {
        waitpid(consumer_pids[i], NULL, 0);
    }
}

int main()
{
    // Main function

    int pipe_fd[2];
    if (pipe(pipe_fd) == -1)
    {
        perror("Error creating pipe");
        exit(EXIT_FAILURE);
    }

    // Initialize semaphores
    consumer_ready = sem_open(CONSUMER_READY, O_CREAT, 0600, MAX_CONSUMERS);
    producer_ready = sem_open(PRODUCER_READY, O_CREAT, 0600, 0);

    if (consumer_ready == SEM_FAILED || producer_ready == SEM_FAILED)
    {
        perror("Semaphore initialization failed");
        exit(EXIT_FAILURE);
    }

    // Start producer
    producer(pipe_fd);

    // Clean up resources

    if (sem_close(consumer_ready) == -1)
    {
        perror("Error closing consumer semaphore");
        exit(EXIT_FAILURE);
    }

    if (sem_close(producer_ready) == -1)
    {
        perror("Error closing producer semaphore");
        exit(EXIT_FAILURE);
    }
    if (sem_close(producer_ready) == -1)
    {
        perror("Error closing producer semaphore");
        exit(EXIT_FAILURE);
    }

    if (sem_unlink(CONSUMER_READY) == -1)
    {
        perror("Error unlinking consumer semaphore");
        exit(EXIT_FAILURE);
    }

    if (sem_unlink(PRODUCER_READY) == -1)
    {
        perror("Error unlinking producer semaphore");
        exit(EXIT_FAILURE);
    }

    return 0;
}