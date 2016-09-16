#include <cstdio>
#include <cstdlib>
#include <omp.h>

int main(int argc, char **argv)
{
	if(argc<3)
	{
		printf("Usage: %s <num_threads> <iters>\n", argv[0]);
		return 1;
	}
	int num_threads = atoi(argv[1]);
	int iters = atoi(argv[2]);
	int correct_cnt = 0;
	unsigned int *seed = new unsigned int[num_threads];
	for(int i=0; i<num_threads; ++i)
		seed[i] = i;
	#pragma omp parallel for reduction(+:correct_cnt) num_threads(num_threads)
	for(int i = 0; i < iters; ++i)
	{
		double x = rand_r(&seed[omp_get_thread_num()]) / (double)RAND_MAX;
		double y = rand_r(&seed[omp_get_thread_num()]) / (double)RAND_MAX;
		double d = x*x+y*y;
		if(d <= 1)
			correct_cnt++;
	}
	
	double pi = 4.0*correct_cnt/iters;
	printf("PI = %.10lf\n", pi);
	return 0;
}
