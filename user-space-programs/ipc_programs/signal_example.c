/*************************************
 ***                               ***
 ***  Author: Nirav Shah           ***
 ***  Email: nsnirav12@gmail.com   ***
 ***  Date: 23rd Dec 2017          ***
 ***                               *** 
 *************************************/

#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include <signal.h>

static void signal_handler (int sig, siginfo_t *siginfo, void *context)
{
	printf ("Signal no %ld Sending PID: %ld, UID: %ld\n",
			(long)siginfo->si_signo, 
			(long)siginfo->si_pid, (long)siginfo->si_uid);
}

static void sig_hdl (int sig)
{
	printf("Signal received %d\n", sig);
}

int main(void)
{
	struct sigaction act;
	sigset_t mask;

	signal(SIGHUP, sig_hdl);

	memset (&act, '\0', sizeof(act));
 
	/* Use the sa_sigaction field because the handles has two additional parameters */
	act.sa_sigaction = &signal_handler;
 
	/* The SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler. */
	act.sa_flags = SA_SIGINFO;

	/* masking signal while signal handler is running*/
	sigemptyset (&mask);
	sigaddset (&mask, SIGINT);

	act.sa_mask = mask; 
 
	if (sigaction(SIGTERM, &act, NULL) < 0) {
		perror ("sigaction");
		return 1;
	}

	memset (&act, '\0', sizeof(act));

	act.sa_handler = SIG_IGN;

	if (sigaction(SIGQUIT, &act, NULL) < 0) {
		perror ("sigaction");
		return 1;
	}

	while (1)
		sleep (10);
 
	return 0;
}
