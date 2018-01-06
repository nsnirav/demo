/*************************************
 ***                               ***
 ***  Author: Nirav Shah           ***
 ***  Email: nsnirav12@gmail.com   ***
 ***  Date: 3rd Jan 2018           ***
 ***                               *** 
 *************************************/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

static int glob = 0;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

static void *                   /* Loop ’arg’ times incrementing ’glob’ */
threadFunc(void *arg) {
	int loops = *((int *) arg);
	int loc, j;

	for (j = 0; j < loops; j++)
	{
		pthread_mutex_lock(&mtx);
		loc = glob;
		loc++;
		glob = loc;
		pthread_mutex_unlock(&mtx);
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	pthread_t t1, t2;
	int loops, s;

	loops = (argc > 1) ?  atoi(argv[1]) : 10000000;

	s = pthread_create(&t1, NULL, threadFunc, &loops);
	if (s != 0) {
		printf("pthread_create failed\n");
                exit(1);
	}

	s = pthread_create(&t2, NULL, threadFunc, &loops);
	if (s != 0) {
		printf("pthread_create failed\n");
                exit(1);
	}

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	printf("glob = %d\n", glob);

}
