#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
	while(1)
	{
		char command[100];
		int i,j,status;
		pid_t pid;

		printf("Enter command :");
		fgets(command,sizeof command,stdin);

		pid=fork();
		if(pid==-1)
		{
			perror("Error in forking");
		}
		else if(pid == 0)
		{
			if(strcmp(command,"jobs") == 0)
			{
				printf("backgorund jobs will be displayed here");
			}
			else if(strcmp(command,"start") == 0)
			{
				printf("start");
			}
			else
			{
				execl("/bin/sh","sh","-c",command,(char *) NULL);
				_exit(127);
			}
		}
		else
		{
			waitpid(pid,&status,0);
			continue;
		}
	}
	return 0;
}