#include <cstdio>
#include <pthread.h>
#include <cstdlib>
#include <ctime>

int total_cnt = 0;

struct tinfo
{
	pthread_t thread_id;
	int iters;
	pthread_mutex_t *mutex;
};

void* thread_func(void *arg)
{
	tinfo *info = (tinfo*)arg;
	unsigned int seed = (unsigned int)info->thread_id;
	int correct_cnt = 0;
	for(int i=0; i<info->iters; ++i)
	{
		double x = rand_r(&seed) / (double)RAND_MAX;
		double y = rand_r(&seed) / (double)RAND_MAX;
		double d = x*x+y*y;
		if(d <= 1)
			correct_cnt++;
	}
	// aggregate results
	pthread_mutex_lock(info->mutex);
	total_cnt += correct_cnt;
	pthread_mutex_unlock(info->mutex);
	return 0;
}

int main(int argc, char **argv)
{
	if(argc<3)
	{
		printf("Usage: %s <num_threads> <iters>\n", argv[0]);
		exit(1);
	}
	srand(time(NULL));
	int num_threads = atoi(argv[1]);
	int iters = atoi(argv[2]);
	int iters_per_thread = iters/num_threads;
	
	tinfo *info = new tinfo[num_threads];
	pthread_mutex_t mutex;
	pthread_mutex_init(&mutex, 0);
	
	// create threads
	for(int i = 0; i < num_threads; ++i)
	{
		info[i].iters = iters_per_thread;
		info[i].mutex = &mutex;
		
		if(pthread_create(&info[i].thread_id, NULL, &thread_func, &info[i]) != 0)
		{
			perror("pthread_create");
			exit(1);
		}
	}
	
	// join threads
	for(int i = 0; i < num_threads; ++i)
	{
		if(pthread_join(info[i].thread_id, NULL) != 0)
		{
			perror("pthread_join");
			exit(1);
		}
	}

	double pi = 4.0*total_cnt/iters;
	printf("PI = %.10lf\n", pi);
	
	return 0;
}
