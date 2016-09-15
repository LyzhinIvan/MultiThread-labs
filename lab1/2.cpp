#include <cstdio>
#include <random>
using namespace std;

int main(int argc, char **argv)
{
	if(argc<2)
	{
		printf("Usage: %s <iters>\n", argv[0]);
		return 1;
	}
	int iters = atoi(argv[1]);
	int correct_cnt = 0;
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> dis(0, 1);
	for(int i=0; i<iters; ++i)
	{
		double x = dis(gen);
		double y = dis(gen);
		double d = x*x+y*y;
		if(d <= 1)
			correct_cnt++;
	}
	double pi = 4.0*correct_cnt/iters;
	printf("PI = %.10lf\n", pi);
	return 0;
}
