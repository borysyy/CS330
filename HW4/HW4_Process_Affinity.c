/*
Written by: Slava Borysyuk
Course: CS330
09/28/2023
HW4_Process_Affinity
Output:
Single processor performance - 
Elapsed Time: 0 seconds and 15970 microseconds
Multiprocessor performance - 
Elapsed Time: 0 seconds and 8018 microseconds
*/
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>

struct timeval start_time, end_time;

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

int main(void)
{
    int num_processes = 4;
    int squared = 0;

    pid_t pid[num_processes];

    // Start timer for single processor test
    starttimer();

    // Set processor affinity for the current process to CPU 0
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    CPU_SET(0, &cpu_set);
    sched_setaffinity(0, sizeof(cpu_set), &cpu_set);

    // Loop thats forking several processes
    for (int i = 0; i < num_processes; i++)
    {
        pid[i] = fork();

        if (pid[i] == 0)
        {
            // Loop that loops a million times while squaring a variable
            for (int i = 0; i < 1000000; i++)
            {
                squared = i * i;
            }
            exit(0);
        }
    }

    // Loop that waits for all the child processes to finish
    for (int i = 0; i < num_processes; i++)
    {
        wait(NULL);
    }

    // End timer for single processor test
    endtimer();

    printf("Single processor performance - \n");
    printtime();



    // Start timer for multiprocessor test
    starttimer();

    // Loop thats forking several processes
    for (int i = 0; i < num_processes; i++)
    {
        pid[i] = fork();

        if (pid[i] == 0)
        {

            // Loop that loops a million times while squaring a variable
            for (int i = 0; i < 1000000; i++)
            {
                squared = i * i;
            }
            exit(0);
        }
    }

    // Loop that waits for all the child processes to finish
    for (int i = 0; i < num_processes; i++)
    {
        wait(NULL);
    }

    // End timer for multiprocessor test
    endtimer();

    printf("Multiprocessor performance - \n");
    printtime();

    return 0;
}
