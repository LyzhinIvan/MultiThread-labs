#include <pthread.h>
#include <semaphore.h>
#include <cstdio>
#include <cstdlib>

class Barrier
{
	const unsigned int numThreads;
	pthread_mutex_t mutex;
	unsigned int waitingThreads;
	sem_t sem;

public:
	Barrier(unsigned int numThreads) : numThreads(numThreads)
	{
		waitingThreads = 0;
		pthread_mutex_init(&mutex, 0);
		sem_init(&sem, 0, 0);
	}

	void wait()
	{
		pthread_mutex_lock(&mutex);
		waitingThreads++;
		pthread_mutex_unlock(&mutex);
		if(waitingThreads == numThreads)
		{
			waitingThreads = 0;
			for(unsigned int i=0; i<numThreads; ++i)
				sem_post(&sem);
		} 
		sem_wait(&sem);
	}
};

struct tinfo
{
	pthread_t threadId;
	Barrier *barrier;
};

void* thread_func(void *arg)
{
	tinfo *info = (tinfo*)arg;
	printf("[%lu]: Before barrier\n", info->threadId);
	info->barrier->wait();
	printf("[%lu]: After barrier\n", info->threadId);
	return 0;
}

int main(int argc, char **argv)
{
	if (argc<2)
	{
		printf("Usage: %s <num_threads>\n", argv[0]);
		return 1;
	}
	int numThreads = atoi(argv[1]);
	Barrier barrier(numThreads);
	tinfo *info = new tinfo[numThreads];
	// Stage 1
	for(int i=0; i<numThreads; ++i) {
		info[i].barrier = &barrier;
		pthread_create(&info[i].threadId, NULL, &thread_func, &info[i]);
	}
	for(int i=0; i<numThreads; ++i)
		pthread_join(info[i].threadId, 0);
	return 0;
}