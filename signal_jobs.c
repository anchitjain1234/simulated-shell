#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
int pid_counter = 0;
int checkampersand(char *command)
{
	int i;
	char *temp;
	temp=malloc(sizeof(command)+1);
	strcpy(temp,command);
	for (i = strlen(temp)-1; i >=0; --i)
	{
		if (temp[i]==' ')
		{
			continue;
		}
		else if(temp[i]=='&')
		{
			return 1;
		}
		else
		{
			// Case when some other character is found i.e no & is appended
			return 0;
		}
	}
	return 0;
}

int main(int argc, char const *argv[])
{
	int pids[100];
	while(1)
	{
		char command[100];
		int i,j,status;
		pid_t pid;

		printf("Enter command :");
		fgets(command,sizeof command,stdin);

		char *pos;
		if ((pos=strchr(command, '\n')) != NULL)
    		*pos = '\0';

		int check = checkampersand(command);
		pid=fork();
		if(pid==-1)
		{
			perror("Error in forking");
		}
		else if(pid == 0)
		{
			if(strcmp(command,"jobs") == 0)
			{
				for (i = 0; i < pid_counter; ++i)
				{
					printf("%d\n",pids[i]);
				}
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
			//Not wait for child to exit if ampersand is appended in last.
			if(!check)
			{
				waitpid(pid,&status,0);
			}
			else
			{
				pids[pid_counter]=pid;
				pid_counter++;
			}
			continue;
		}
	}
	return 0;
}