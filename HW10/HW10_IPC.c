#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <semaphore.h>
#include <errno.h>

#define MAX_WORK_DESCRIPTION_LENGTH 50
#define MAX_CONSUMERS 5

#define CONSUMER_READY "/consumer_ready"
#define PRODUCER_READY "/producer_ready"

sem_t *consumer_ready;
sem_t *producer_ready;

struct msg_buffer
{
    long msg_type;
    char work_description[MAX_WORK_DESCRIPTION_LENGTH];
    int execution_time; // Random execution time
};

void log_message(const char *prefix, const char *work, int execution_time)
{
    struct timespec timestamp;
    clock_gettime(CLOCK_REALTIME, &timestamp);

    printf("%s %d: '%s' for %d nanoseconds sent at %ld seconds\n",
           prefix, getpid(), work, execution_time, timestamp.tv_sec);
}

void producer(int msgq_id)
{
    srand(time(NULL));
    FILE *work_file = fopen("work.txt", "r");
    if (!work_file)
    {
        perror("Error opening work file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_WORK_DESCRIPTION_LENGTH];
    printf("Hi");
    while (fgets(line, sizeof(line), work_file) != NULL)
    {
        printf("Sazzadio");
        sem_wait(consumer_ready);

        struct msg_buffer message;
        message.msg_type = 1; // Message type for consumers

        // Read work description from file
        size_t length = strlen(line);
        if (line[length - 1] == '\n')
        {
            line[length - 1] = '\0'; // Removing the newline character
        }
        strncpy(message.work_description, line, sizeof(line));

        printf("sober");

        // Generate random execution time
        message.execution_time = rand() % 5 + 1; // Range: 1-5 seconds

        // Send message to the queue
        if (msgsnd(msgq_id, &message, sizeof(message) - sizeof(long), 0) == -1)
        {
            perror("Error sending message");
            exit(EXIT_FAILURE);
        }

        printf("SCOTTY");

        log_message("Producer", message.work_description, message.execution_time);
        sleep(rand() % 3); // Random wait before sending next work

        sem_post(producer_ready);
    }

    fclose(work_file);
}

void consumer(int msgq_id)
{

    char log_file_name[30];
    sprintf(log_file_name, "consumer_%d.log", getpid()); // Use process ID in log file name

    FILE *log_file = fopen(log_file_name, "w");
    if (!log_file)
    {
        perror("Error opening log file");
        exit(EXIT_FAILURE);
    }

    while (1)
    {

        sem_wait(producer_ready);

        struct msg_buffer message;
        if (msgrcv(msgq_id, &message, sizeof(message) - sizeof(long), 1, 0) == -1)
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

        log_message("Consumer", message.work_description, message.execution_time);

        // Log work details to the file
        fprintf(log_file, "%s %d: '%s' for %d nanoseconds sent at %ld seconds\n",
                "Consumer", getpid(), message.work_description, message.execution_time, time(NULL));
        fflush(log_file); // Ensure the data is written to the file immediately

        sleep(message.execution_time); // Simulate work execution

        sem_post(consumer_ready);
    }

    fclose(log_file);
}

int main()
{
    int msgq_id = msgget(IPC_PRIVATE, IPC_CREAT | 0666);
    if (msgq_id == -1)
    {
        perror("Error creating message queue");
        exit(EXIT_FAILURE);
    }

    consumer_ready = sem_open(CONSUMER_READY, O_CREAT, 0600, MAX_CONSUMERS);
    producer_ready = sem_open(PRODUCER_READY, O_CREAT, 0600, 0);

    if (consumer_ready == SEM_FAILED || producer_ready == SEM_FAILED)
    {
        perror("sem_open error");
        exit(1);
    }

    pid_t producer_pid = fork();
    if (producer_pid == -1)
    {
        perror("Error forking producer process");
        exit(EXIT_FAILURE);
    }
    else if (producer_pid == 0)
    {
        producer(msgq_id);
        exit(EXIT_SUCCESS);
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

    // Wait for all child processes to complete
    waitpid(producer_pid, NULL, 0);
    for (int i = 0; i < MAX_CONSUMERS; ++i)
    {
        waitpid(consumer_pids[i], NULL, 0);
    }

    // Clean up resources
    if (msgctl(msgq_id, IPC_RMID, NULL) == -1)
    {
        perror("Error removing message queue");
    }

    if (sem_close(consumer_ready) == -1)
    {
        perror("error");
        exit(1);
    }

    if (sem_close(producer_ready) == -1)
    {
        perror("error");
        exit(1);
    }

    if (sem_unlink(CONSUMER_READY) == -1)
    {
        perror("error");
        exit(1);
    }

    if (sem_unlink(PRODUCER_READY) == -1)
    {
        perror("error");
        exit(1);
    }

    return 0;
}
