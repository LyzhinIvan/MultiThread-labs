#include <cstdio>
#include <pthread.h>
#include <cstdlib>

void* thread_func(void *arg)
{
	printf("Hello, world!\n");
	return 0;
}

int main(int argc, char **argv)
{
	if(argc<2)
	{
		printf("Usage: %s <num_threads>\n", argv[0]);
		exit(1);
	}
	
	int num_threads = atoi(argv[1]);
	pthread_t *thread_ids = new pthread_t[num_threads];
	for(int i = 0; i < num_threads; ++i)
		if(pthread_create(&thread_ids[i], NULL, &thread_func, NULL) != 0)
		{
			perror("pthread_create");
			exit(1);
		}
	
	for(int i = 0; i < num_threads; ++i)
		if(pthread_join(thread_ids[i], NULL) != 0)
		{
			perror("pthread_join");
			exit(1);
		}
	
	printf("Goodbye, world!\n");
	delete [] thread_ids;
	
	return 0;
}
