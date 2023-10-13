/*
Written by: Slava Borysyuk
Course: CS330
10/13/2023
HW6_Scheduling
Output:
Experiment without lowering thread priority:
Elapsed Time: 0 seconds and 2965 microseconds

Experiment with lowering thread priority:
Elapsed Time: 0 seconds and 4276 microseconds

Experiment without lowering process priority:
Elapsed Time: 0 seconds and 1684 microseconds

Experiment with lowering process priority:
Elapsed Time: 0 seconds and 1737 microseconds
*/

#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

struct timeval start_time, end_time;
int squared = 0;

// Function to start the timer
void starttimer()
{
    gettimeofday(&start_time, NULL);
}

// Function to stop the timer
void endtimer()
{
    gettimeofday(&end_time, NULL);
}

// Function to print the elapsed time
void printtime()
{
    long start_MS = start_time.tv_sec * 1000000 + start_time.tv_usec;
    long end_MS = end_time.tv_sec * 1000000 + end_time.tv_usec;
    long totalelapsed = end_MS - start_MS;

    long seconds = totalelapsed / 1000000;
    long microseconds = totalelapsed % 1000000;

    printf("Elapsed Time: %ld seconds and %ld microseconds\n", seconds, microseconds);
}

void *thread_function(void *arg)
{
    for (int i = 0; i < 250000; i++)
    {
        squared = i * i;
    }
    return NULL;
}

int main(void)
{

    // Experiment without lowering thread priority
    starttimer();
    pthread_t threads[4];

    for (int i = 0; i < 4; i++)
    {
        pthread_create(&threads[i], NULL, thread_function, NULL);
    }

    for (int i = 0; i < 4; i++)
    {
        pthread_join(threads[i], NULL);
    }

    endtimer();
    printf("Experiment without lowering thread priority: \n");
    printtime();
    printf("\n");

    // Experiment with lowering thread priority
    starttimer();

    pthread_t lowered_threads[4];

    for (int i = 0; i < 4; i++)
    {
        pthread_create(&lowered_threads[i], NULL, thread_function, NULL);
    }

    // Set one thread's priority to a lower value using nice
    int nice_ret_thread = nice(10); // Decrease the thread's priority

    for (int i = 0; i < 4; i++)
    {
        pthread_join(lowered_threads[i], NULL);
    }

    endtimer();
    printf("Experiment with lowering thread priority: \n");
    printtime();
    printf("\n");

    // Experiment without lowering process priority
    starttimer();

    for (int i = 0; i < 4; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            // Child process
            for (int j = 0; j < 250000; j++)
            {
                squared = j * j;
            }
            return 0;
        }
    }

    for (int i = 0; i < 4; i++)
    {
        wait(NULL);
    }

    endtimer();
    printf("Experiment without lowering process priority: \n");
    printtime();
    printf("\n");

    // Experiment with lowering process priority
    starttimer();

    for (int i = 0; i < 4; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            // Child process
            for (int j = 0; j < 250000; j++)
            {
                squared = j * j;
            }
            return 0;
        }
    }

    // Set one child process's priority to a lower value using nice
    int nice_ret_process = nice(10); // Decrease the process's priority

    for (int i = 0; i < 4; i++)
    {
        wait(NULL);
    }

    endtimer();
    printf("Experiment with lowering process priority: \n");
    printtime();
    printf("\n");

    return 0;
}
