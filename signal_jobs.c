#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
	while(1)
	{
		char command[100];
		int i,j,status;
		pid_t pid;

		printf("Enter command :");
		fgets(command,sizeof command,stdin);
		int check = checkampersand(command);
		pid=fork();
		if(pid==-1)
		{
			perror("Error in forking");
		}
		else if(pid == 0)
		{
			if(strcmp(command,"jobs\n") == 0)
			{
				printf("backgorund jobs will be displayed here");
			}
			else if(strcmp(command,"start\n") == 0)
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
			if(!check)
			{
				waitpid(pid,&status,0);
			}
			continue;
		}
	}
	return 0;
}