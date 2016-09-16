#include <pthread.h>
#include <semaphore.h>
#include <cstdio>
#include <cstdlib>
#include <unordered_map>

class Barrier
{
	const unsigned int numThreads;
	pthread_mutex_t mutex;
	unsigned int waitingThreads;
	std::unordered_map<pthread_t, sem_t*> sem;

public:
	Barrier(unsigned int numThreads) : numThreads(numThreads)
	{
		waitingThreads = 0;
		pthread_mutex_init(&mutex, 0);
	}

	void wait(pthread_t threadId)
	{
		pthread_mutex_lock(&mutex);
		waitingThreads++;
		// register threadId
		if(sem.find(threadId)==sem.end())
		{
			sem[threadId] = new sem_t();
			sem_init(sem[threadId], 0, 0);
		}
		if(waitingThreads == numThreads)
		{
			printf("[%lu]: All threads on barrier\n", threadId);
			waitingThreads = 0;
			for(auto p : sem)
				sem_post(p.second);
		}
		pthread_mutex_unlock(&mutex);
		sem_wait(sem[threadId]);
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
	printf("[%lu]: Before barriers\n", info->threadId);
	info->barrier->wait(info->threadId);
	printf("[%lu]: After first barrier\n", info->threadId);
	info->barrier->wait(info->threadId);
	printf("[%lu]: After second barrier\n", info->threadId);
	info->barrier->wait(info->threadId);
	printf("[%lu]: After third barrier\n", info->threadId);
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
	for(int i=0; i<numThreads; ++i) {
		info[i].barrier = &barrier;
		pthread_create(&info[i].threadId, NULL, &thread_func, &info[i]);
	}
	for(int i=0; i<numThreads; ++i)
		pthread_join(info[i].threadId, 0);
	return 0;
}