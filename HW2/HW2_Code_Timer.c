/*
Written by: Slava Borysyuk
Course: CS330
09/14/2023
HW2_Code_Timer
Output:
Elapsed Time: 2 seconds and 916723 microseconds
*/

#include <stdio.h>
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
    starttimer();
   
   for(int i = 0; i < 1000000; i++)
   {
        printf("Hello\n");
        printf("HW\n");
        printf("Test\n");
   }

    endtimer();

    printtime();

    return 0;
}
