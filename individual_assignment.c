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
	if(sig==SIGINT){
		printf("\nPROCESS INTERRUPTED!\n");
		printf("---------------------------\n");
		fflush(stdout);
		_exit(1);
	}
}
void child_process(int id, int readpipefds)
{
	char message[BUFF_SIZE];
	ssize_t n = read(readpipefds,message,sizeof(message)-1);
	if(n<0){perror("read"); exit(1);}
	message[n]='\0';
	printf("Child %d received \"%s\"\n",id, message);
	printf("\n----------------------------------\n");
	close(readpipefds);
	exit(0);
}
int main(void){
	int pipefds[2];
	pid_t pid;
	char message[BUFF_SIZE];
	int numchild;

	signal(SIGINT, sig_handler);
	if(signal(SIGINT,sig_handler)==SIG_ERR){
		perror("signal");
		exit(1);
	}

	printf("How many child: ");
	scanf("%d%*c", &numchild);
	printf("\n==================================");

	printf("\nParent process: Type a message: ");
	fflush(stdout);
	fgets(message,BUFF_SIZE,stdin);
	message[strcspn(message, "\n")] = '\0';
	printf("\n----------------------------------");
	for(int i=0; i<numchild; i++){
		if(pipe(pipefds)==-1){
			perror("pipe");
			exit(1);
		}
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
		printf("\nParent sent \"%s\" to child %d\n", message, i+1);
		write(pipefds[1], message, strlen(message)+1);
		close(pipefds[1]);
		wait(NULL);
	}
	printf("All child has received message.\n");
	return 0;
}
