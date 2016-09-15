#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdlib>

void childFunc()
{
	printf("PID: %d\n", getpid());
	exit(0);
}

int main(int argc, char **argv)
{
	pid_t child1 = fork();
	if(child1==0)
		childFunc();
	pid_t child2 = fork();
	if(child2==0)
		childFunc();
	wait(0);
	wait(0);
	printf("Hello, world!\n");
	return 0;
}
