#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <ctime>
using namespace std;

int main(int argc, char *argv[])
{
    srand(time(nullptr));
    for (int i = 0; i < 20; i++)
    {
        pid_t p = fork();
        if (p == 0)
        {
            usleep(rand() % 100000);
            cout << i << " ";
            return 0;
        }
    }

    while (wait(NULL) > 0)
        ;
    cout << endl;
    return 0;
}