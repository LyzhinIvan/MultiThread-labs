	#include <semaphore.h>
#include <pthread.h>
#include <cstdio>
#include <cstdlib>
#include <stack>
#include <queue>

template< class T >
class Stack
{
	std::stack<T> st;
	pthread_mutex_t mutex;
	sem_t sem;
	
public:
	Stack()
	{
		if(pthread_mutex_init(&mutex, 0) != 0)
		{
			perror("mutex_init");
			exit(1);
		}
		if(sem_init(&sem, 0, 0) != 0)
		{
			perror("sem_init");
			exit(1);
		}		
	}
	
	void push(T x)
	{
		pthread_mutex_lock(&mutex);
		st.push(x);
		pthread_mutex_unlock(&mutex);
		sem_post(&sem);
	}
	
	T pop()
	{
		sem_wait(&sem);
		pthread_mutex_lock(&mutex);
		T ret = st.top();
		st.pop();
		pthread_mutex_unlock(&mutex);
		return ret;
	}
};

template< class T >
struct Queue
{
	std::queue<T> q;
	pthread_mutex_t mutex;
	sem_t sem;
	
public:
	Queue()
	{
		if(pthread_mutex_init(&mutex, 0) != 0)
		{
			perror("mutex_init");
			exit(EXIT_FAILURE);
		}
		if(sem_init(&sem, 0, 0) != 0)
		{
			perror("sem_init");
			exit(EXIT_FAILURE);
		}		
	}
	
	void push(T x)
	{
		pthread_mutex_lock(&mutex);
		q.push(x);
		pthread_mutex_unlock(&mutex);
		sem_post(&sem);
	}
	
	T pop()
	{
		sem_wait(&sem);
		pthread_mutex_lock(&mutex);
		T ret = q.front();
		q.pop();
		pthread_mutex_unlock(&mutex);
		return ret;
	}
};

struct tinfo
{
	pthread_t thread_id;
	int from;
	int cnt;
};

Stack<int> stack;
Queue<int> queue;

void* stack_push_func(void *arg)
{
	tinfo info = *(tinfo*)arg;
	for(int x = info.from; x < info.from + info.cnt; ++x)
	{
		printf("%lu: Push %d to stack\n", info.thread_id, x);
		stack.push(x);
	}
	return 0;
}

void* stack_pop_func(void *arg)
{
	tinfo info = *(tinfo*)arg;
	for(int i = 0; i < info.cnt; ++i)
	{
		int x = stack.pop();
		printf("%lu: Pop %d from stack\n", info.thread_id, x);
	}
	return 0;
}

void* queue_push_func(void *arg)
{
	tinfo info = *(tinfo*)arg;
	for(int x = info.from; x < info.from + info.cnt; ++x)
	{
		printf("%lu: Push %d to queue\n", info.thread_id, x);
		queue.push(x);
	}
	return 0;
}

void* queue_pop_func(void *arg)
{
	tinfo info = *(tinfo*)arg;
	for(int i = 0; i < info.cnt; ++i)
	{
		int x = queue.pop();
		printf("%lu: Pop %d from queue\n", info.thread_id, x);
	}
	return 0;
}

int main(int argc, char **argv)
{
	if(argc<3)
	{
		printf("Usage: %s <num_threads> <cnt>\n", argv[0]);
		return 1;
	}
	int num_threads = atoi(argv[1]);
	int cnt = atoi(argv[2]);
	tinfo *threads = new tinfo[num_threads*4];
	for(int i=0; i<num_threads; ++i)
	{
		threads[i*4].from = i*cnt;
		threads[i*4].cnt = cnt;
		pthread_create(&threads[i*4].thread_id, NULL, &stack_push_func, &threads[i*4]);
		
		threads[i*4+1].cnt = cnt;
		pthread_create(&threads[i*4+1].thread_id, NULL, &stack_pop_func, &threads[i*4+1]);
		
		threads[i*4+2].from = i*cnt;
		threads[i*4+2].cnt = cnt;
		pthread_create(&threads[i*4+2].thread_id, NULL, &queue_push_func, &threads[i*4+2]);
		
		threads[i*4+3].cnt = cnt;
		pthread_create(&threads[i*4+3].thread_id, NULL, &queue_pop_func, &threads[i*4+3]);
	}
	for(int i=0; i<num_threads*4; ++i)
		pthread_join(threads[i].thread_id, NULL);
	return 0;
}
