#include <cstdio>
#include <unistd.h>
#include <random>
using namespace std;

int main(int argc, char **argv)
{
	if(argc<3)
	{
		printf("Usage: %s <procs> <iters>\n", argv[0]);
		return 1;
	}
	int procs = atoi(argv[1]);
	int iters = atoi(argv[2]);
	int iters_per_proc = iters/procs;
	
	int pipefd[2];
	if(pipe(pipefd) == -1)
	{
		perror("pipe");
		exit(EXIT_FAILURE);
	}
	
	for(int p = 0; p < procs; ++p)
	{
		int pid = fork();
		if(pid == -1)
		{
			perror("fork");
			exit(EXIT_FAILURE);
		}
		if(pid==0) // child process
		{
			close(pipefd[0]); // close unused read end
			int correct_cnt = 0;
			random_device rd;
			mt19937 gen(rd());
			uniform_real_distribution<> dis(0, 1);
			for(int i=0; i<iters_per_proc; ++i)
			{
				double x = dis(gen);
				double y = dis(gen);
				double d = x*x+y*y;
				if(d <= 1)
					correct_cnt++;
			}
			write(pipefd[1], &correct_cnt, sizeof(int)); // send to parent via pipe
			close(pipefd[1]);
			exit(0);
		}
	}
	
	int total_correct_cnt = 0;
	for(int p = 0; p < procs; ++p) // gather results from child processes
	{
		int child_cnt;
		read(pipefd[0], &child_cnt, sizeof(int)); // receive from child via pipe
		total_correct_cnt += child_cnt;
	}
	close(pipefd[0]);
	close(pipefd[1]);
	
	double pi = 4.0*total_correct_cnt/iters;
	printf("PI = %.10lf\n", pi);
	return 0;
}
