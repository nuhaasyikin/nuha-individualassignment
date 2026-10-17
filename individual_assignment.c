#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define BUFF_SIZE 256

void sig_handler(int sig)
{
	if(sig==SIGINT)
		printf("This is a special signal handler for SIGINT\n");
}
void child_process(int id, int readpipefds)
{
	char message[BUFF_SIZE];
	read(readpipefds,message,sizeof(message)-1);
	printf("Child %d received \"%s\"\n",id, message);
	close(readpipefds);
	exit(0);
}
int main(void){
	int pipefds[2];
	pid_t pid;
	char message[BUFF_SIZE];
	int numchild;

	signal(SIGINT, sig_handler);

	printf("How many child: ");
	scanf("%d%*c", &numchild);

	printf("Parent process: Type a message: ");
	fflush(stdout);
	fgets(message,BUFF_SIZE,stdin);
	message[strcspn(message, "\n")] = '\0';
	for(int i=0; i<numchild; i++){
		pipe(pipefds);
		fflush(stdout);
		pid = fork();
		if(pid<0){
			perror("fork");
			exit(1);
		}
		if(pid==0){
			close(pipefds[1]);
			child_process(i+1, pipefds[0]);
		}
		close(pipefds[0]);
		printf("Parent sent \"%s\" to child %d\n", message, i+1);
		write(pipefds[1], message, strlen(message)+1);
		close(pipefds[1]);
		wait(NULL);
	}
	for(int j=0; j<numchild; j++){
		wait(NULL);
	}
	return 0;
}
