#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <random>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <mqueue.h>
using namespace std;

const int MQ_MSGSIZE = 10;

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
	
	struct mq_attr attr;
	attr.mq_maxmsg = procs;
	attr.mq_msgsize = MQ_MSGSIZE;
	mqd_t mqd = mq_open("/pi_queue", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, &attr);
	if(mqd == (mqd_t) -1)
	{
		perror("mq_open");
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
			char buf[MQ_MSGSIZE];
			sprintf(buf, "%d", correct_cnt);
			mq_send(mqd, buf, attr.mq_msgsize, 0); // send to parent via queue
			mq_close(mqd);
			exit(0);
		}
	}
	char buf[MQ_MSGSIZE];
	int total_correct_cnt = 0;
	for(int p = 0; p < procs; ++p) // gather results from child processes
	{
		mq_receive(mqd, buf, attr.mq_msgsize, NULL); // receive from parent via queue
		int child_cnt = atoi(buf);
		total_correct_cnt += child_cnt;
	}
	mq_close(mqd);
	mq_unlink("/pi_queue");
	
	
	double pi = 4.0*total_correct_cnt/iters;
	printf("PI = %.10lf\n", pi);
	return 0;
}
