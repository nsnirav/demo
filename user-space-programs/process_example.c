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

int main(void)
{

    printf("My pid:%d ppid %d\n", getpid(), getppid());

    return 0;
}
