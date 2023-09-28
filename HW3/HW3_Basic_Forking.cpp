/*
Written by: Slava Borysyuk
Course: CS330
09/19/2023
HW3_Basic_Forking
Output:
ABCDEFGHIJKLMNOPQRSTUVWXYZ
ABCDEFGHIJKLMNOPQRSTUVWXYZ
ABCDEFGHIJKLMNOPQRSTUVWXYZ
ABCDEFGHIJKLMNOPQRSUTWVXYZ
BCDJOPIQHEFWGKLAMSRNVXYTUZ
ABCEFJKLOPRTIXYGHMVSNWUZQD
CDIAVLWUXBTGFEJMHONPSRZYQK
ABCFGDIHEOPMNTVUSXLQJZWYRK
BACFDEGIKHLNMJPOQRSTUVWXYZ
ABCDEFGHIJKLMNOPQRSTUVXYWZ
ABCDEFGHIJKLMNOPQRSTUVWXYZ
ABCDEFGHIKJLMNOPQRSTUVWXYZ
*/

#include <iostream>
#include <cstdlib>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

int main(void)
{

    for (int i = 0; i < 12; i++)
    {
        for (int j = 0; j < 26; j++)
        {
            pid_t pid = fork();

            if (pid == 0)
            {
                char letter = 'A' + j;
                cout << letter;
                exit(0);
            }
        }

        while (wait(NULL) > 0)
            ;
        cout << "\n";
    }

    return 0;
}
