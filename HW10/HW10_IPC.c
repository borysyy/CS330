/*
Written by: Slava Borysyuk
Course: CS330
11/28/2023
HW10_IPC
Output:
Producer 9378: 'work1' for 91591472 nanoseconds sent at 1701211353 seconds
Consumer 9379: 'work1' for 91591472 nanoseconds sent at 1701211353 seconds
Producer 9378: 'work2' for 99103954 nanoseconds sent at 1701211354 seconds
Producer 9378: 'work3' for 172057677 nanoseconds sent at 1701211354 seconds
Consumer 9380: 'work2' for 99103954 nanoseconds sent at 1701211354 seconds
Producer 9378: 'work4' for 455899238 nanoseconds sent at 1701211354 seconds
Consumer 9381: 'work3' for 172057677 nanoseconds sent at 1701211354 seconds
Consumer 9382: 'work4' for 455899238 nanoseconds sent at 1701211354 seconds
Producer 9378: 'work5' for 31975438 nanoseconds sent at 1701211354 seconds
Consumer 9383: 'work5' for 31975438 nanoseconds sent at 1701211354 seconds
Producer 9378: 'work6' for 28740710 nanoseconds sent at 1701211354 seconds
Consumer 9379: 'work6' for 28740710 nanoseconds sent at 1701211355 seconds
Producer 9378: 'work7' for 217700671 nanoseconds sent at 1701211355 seconds
Producer 9378: 'work8' for 212966948 nanoseconds sent at 1701211355 seconds
Consumer 9380: 'work7' for 217700671 nanoseconds sent at 1701211355 seconds
Consumer 9381: 'work8' for 212966948 nanoseconds sent at 1701211355 seconds
Producer 9378: 'work9' for 221178313 nanoseconds sent at 1701211355 seconds
Consumer 9382: 'work9' for 221178313 nanoseconds sent at 1701211355 seconds
Producer 9378: 'work10' for 398418387 nanoseconds sent at 1701211355 seconds
Consumer 9383: 'work10' for 398418387 nanoseconds sent at 1701211356 seconds
Producer 9378: 'work11' for 57388921 nanoseconds sent at 1701211356 seconds
Consumer 9379: 'work11' for 57388921 nanoseconds sent at 1701211356 seconds
Producer 9378: 'work12' for 390462895 nanoseconds sent at 1701211356 seconds
Consumer 9380: 'work12' for 390462895 nanoseconds sent at 1701211356 seconds
Producer 9378: 'work13' for 245963976 nanoseconds sent at 1701211357 seconds
Consumer 9381: 'work13' for 245963976 nanoseconds sent at 1701211357 seconds
Producer 9378: 'work14' for 393828786 nanoseconds sent at 1701211357 seconds
Consumer 9382: 'work14' for 393828786 nanoseconds sent at 1701211357 seconds
Producer 9378: 'work15' for 241819262 nanoseconds sent at 1701211357 seconds
Producer 9378: 'work16' for 90388660 nanoseconds sent at 1701211358 seconds
Consumer 9383: 'work15' for 241819262 nanoseconds sent at 1701211358 seconds
Consumer 9379: 'work16' for 90388660 nanoseconds sent at 1701211358 seconds
Producer 9378: 'work17' for 80274532 nanoseconds sent at 1701211358 seconds
Consumer 9380: 'work17' for 80274532 nanoseconds sent at 1701211358 seconds
Producer 9378: 'work18' for 357250267 nanoseconds sent at 1701211358 seconds
Producer 9378: 'work19' for 127815547 nanoseconds sent at 1701211358 seconds
Consumer 9382: 'work19' for 127815547 nanoseconds sent at 1701211358 seconds
Producer 9378: 'work20' for 397285927 nanoseconds sent at 1701211358 seconds
Consumer 9381: 'work18' for 357250267 nanoseconds sent at 1701211358 seconds
Consumer 9383: 'work20' for 397285927 nanoseconds sent at 1701211359 seconds
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <semaphore.h>
#include <fcntl.h>

#define MAX_WORK_DESCRIPTION_LENGTH 50
#define MAX_CONSUMERS 5

#define CONSUMER_READY "/consumer_ready"
#define PRODUCER_READY "/producer_ready"

sem_t *consumer_ready; // Semaphore for consumer synchronization
sem_t *producer_ready; // Semaphore for producer synchronization

struct msg_buffer
{
    long msg_type; // Message type for consumers
    char work_description[MAX_WORK_DESCRIPTION_LENGTH];
    long execution_time;
};

void consumer(int msgq_id)
{
    // Function to simulate consumer behavior

    char log_file_name[30];
    sprintf(log_file_name, "consumer_%d.log", getpid()); // Use process ID in log file name

    while (1)
    {
        // Wait until the producer signals that work is ready
        if (sem_wait(producer_ready) == -1)
        {
            perror("Error waiting for producer semaphore");
            exit(EXIT_FAILURE);
        }

        struct msg_buffer message;
        // Receive message from the message queue
        if (msgrcv(msgq_id, &message, sizeof(message) - sizeof(long), 1, IPC_NOWAIT) == -1)
        {
            perror("Error receiving message");
            exit(EXIT_FAILURE);
        }

        // Trim newline characters if present in work_description
        size_t length = strlen(message.work_description);
        if (message.work_description[length - 1] == '\n')
        {
            message.work_description[length - 1] = '\0';
        }

        // Simulate execution time
        struct timespec sleep_time, remaining_time;
        sleep_time.tv_sec = 0;
        sleep_time.tv_nsec = message.execution_time;
        nanosleep(&sleep_time, &remaining_time);

        // Print and log work details
        printf("%s %d: '%s' for %ld nanoseconds sent at %ld seconds\n",
               "Consumer", getpid(), message.work_description, message.execution_time, time(NULL));

        FILE *log_file = fopen(log_file_name, "a");
        if (!log_file)
        {
            perror("Error opening log file");
            exit(EXIT_FAILURE);
        }

        // Log work details to the file
        fprintf(log_file, "%s %d: '%s' for %ld nanoseconds sent at %ld seconds\n",
                "Consumer", getpid(), message.work_description, message.execution_time, time(NULL));

        fclose(log_file);

        // Signal producer that a consumer is ready for more work
        if (sem_post(consumer_ready) == -1)
        {
            perror("Error signaling consumer semaphore");
            exit(EXIT_FAILURE);
        }
    }
}

void producer(int msgq_id)
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
            consumer(msgq_id);
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
        message.msg_type = 1; // Message type for consumers

        // Read work description from file
        size_t length = strlen(line);
        if (line[length - 1] == '\n')
        {
            line[length - 1] = '\0'; // Removing the newline character
        }
        strncpy(message.work_description, line, sizeof(line));

        // Generate random execution time
        message.execution_time = rand() % 500000000;

        // Send message to the queue
        if (msgsnd(msgq_id, &message, sizeof(message) - sizeof(long), IPC_NOWAIT) == -1)
        {
            perror("Error sending message");
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

    int msgq_id = msgget(IPC_PRIVATE, IPC_CREAT | 0666); // Create a message queue

    if (msgq_id == -1)
    {
        perror("Error creating message queue");
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
    producer(msgq_id);

    // Clean up resources
    if (msgctl(msgq_id, IPC_RMID, NULL) == -1)
    {
        perror("Error removing message queue");
    }

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
