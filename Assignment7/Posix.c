//Hossein Niazmandi
//91304794
//415Hw7
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/wait.h>

#define MAXLINE 1024
#define NUM_OF_TOKENS 50

void stringTokenize(char *org, char *tokens[NUM_OF_TOKENS], int *size)
{
	const char *delim = " \t\n";
	char *token = NULL;
	char *saveptr = NULL;
	int count = 0;
	for(token = strtok_r(org,delim,&saveptr) ; token && count < *size ; token = strtok_r(NULL,delim,&saveptr))
	{
		tokens[count++] = token;
	}
	*size = count-1;
	tokens[count] = '\0';
}

int getNumOfArgs(char *cmd[])
{
	int count = 0;
	while(cmd[count++]);
	return count;
}
void exec(char *cmd[], int bgProc)
{
	pid_t pid;
	pid = fork();
	if(pid == 0)
	{
		char *c;
		int count = 0;
		for(c = cmd[count]; c; ++count,c = cmd[count])
		{
			if(strcmp(c,">") == 0)
			{
				int fd = open(cmd[count+1],O_CREAT|O_WRONLY|O_TRUNC,0600);
				dup2(fd,1);
				cmd[count] = NULL;
			}else if((strcmp(c,">>") == 0))
			{
				int fd = open(cmd[count+1],O_CREAT|O_WRONLY|O_APPEND,0600);
				dup2(fd,1);
				cmd[count] = NULL;
				break;
			}	
			else if(strcmp(c,"2>") == 0)
			{
				int fd = open(cmd[count+1],O_CREAT|O_WRONLY|O_TRUNC,0600);
				//dup2(fd,1);
				dup2(fd,2);
				cmd[count] = NULL;
			}else if((strcmp(c,"2>>") == 0))
			{
				int fd = open(cmd[count+1],O_CREAT|O_WRONLY|O_APPEND,0600);
				//dup2(fd,1);
				dup2(fd,2);
				cmd[count] = NULL;
			}else if(strcmp(c,"<") == 0)
			{
				 int fd = open(cmd[count+1],O_RDONLY,0600);
				dup2(fd,0);
				cmd[count] = NULL;
			}
			else if(strcmp(c,"|") == 0)
			{
				int fd[2];
				pipe(fd);
				pid_t cpid = fork();
				if(cpid == 0)
				{
					dup2(fd[0],0);
					exec(&cmd[count+1],1);
					_exit(0);
				}
				else
				{
					cmd[count] = NULL;
					dup2(fd[1],1);
					printf("\n");
				}
				break;
			}
		}
		int x = getNumOfArgs(cmd);
		if(x == 1)
		{
			execlp(*cmd,*cmd,NULL);
		}else if(x == 2)
		{
			execlp(*cmd,*cmd,cmd[1],NULL);
		}else if(x >=  3)
		{
			execlp(*cmd,*cmd,cmd[1],cmd[2],NULL);
		}
		_exit(0);
	}else
	{
		if(!bgProc)
			waitpid(pid,NULL,0);
	}
}
void sigHandler(int sig, siginfo_t *siginfo, void *context)
{
	wait(NULL);
}
void regSignalHandlers()
{
	struct sigaction act;
	memset (&act, 0, sizeof(act));
	act.sa_sigaction = &sigHandler;
	if (sigaction(SIGCHLD, &act, NULL) < 0) {
	}
 
}
int main(int argc,char *argv[])
{
	regSignalHandlers();
	do
	{
		printf("MyShell>");
		char line[MAXLINE];
		fgets(line,MAXLINE-1,stdin);
		if(strcmp(line,"\n") == 0)
			continue;
		char *tokens[NUM_OF_TOKENS];
		int size = NUM_OF_TOKENS;
		stringTokenize(line,tokens,&size);
		int bgProc = 0;
		if(*(tokens[size]) == '&')
		{
			tokens[size] = NULL;
			bgProc = 1;
		}
		else if(strcmp(tokens[size],"quit") == 0)
			break;
		exec(tokens,bgProc);	
	}while(1);
	return 0;
}
