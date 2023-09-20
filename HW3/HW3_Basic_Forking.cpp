/*
Written by: Slava Borysyuk
Course: CS330
09/19/2023
HW3_Basic_Forking
Output:



*/

#include <iostream>
#include <cstdlib>
#include <sys/wait.h>
#include<unistd.h>

using namespace std;

int main (void)
{


fork();
fork();
fork();
cout << "Hello\n";



}


