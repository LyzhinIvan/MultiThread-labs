#include <cstdio>
#include <cstdlib>
#include <omp.h>

int main(int argc, char **argv)
{
	if(argc<2)
	{
		printf("Usage: %s <num_threads>\n", argv[0]);
		return 1;
	}
	int num_threads = atoi(argv[1]);
	#pragma omp parallel num_threads(num_threads)
	{
		printf("[%d]: Hello, world\n", omp_get_thread_num());
	}
	printf("Goodbye, world!\n");
	return 0;
}
