/*************************************
 ***                               ***
 ***  Author: Nirav Shah           ***
 ***  Email: nsnirav12@gmail.com   ***
 ***  Date: 23rd Dec 2017          ***
 ***                               *** 
 *************************************/


#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main(void)
{
    int pid, status;
    int a = 10;

    printf("My pid:%d ppid %d\n", getpid(), getppid());

    pid = fork();
    if (pid == 0) {
        printf("Hello from Child! a=%d\n", ++a);
        printf("My pid:%d ppid %d\n", getpid(), getppid());
    } else {
        printf("Hello from Parent! a=%d\n", --a); 
        printf("My pid:%d ppid %d child_pid %d\n", getpid(), getppid(), pid);
        sleep(100);
        wait(&status);
    }

    return 0;
}
