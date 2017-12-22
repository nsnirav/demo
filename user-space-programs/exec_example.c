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
    char *args[]={"./test", NULL};
    char *argv[]={"/bin/sh", "-c", "env", NULL};
    char *envp[] = {"USER=beelzebub", NULL};

    printf("My pid:%d ppid %d\n", getpid(), getppid());

    execve(argv[0], &argv[0], envp);
    execlp("ls", "ls", "-lrt", NULL);
    execlp("date", "date", NULL);
    execl("/bin/date", "date", NULL); 
    execl("/bin/date", "", NULL);
    execvp(args[0], args);


    printf("Exiting process\n");
    return 0;
}
