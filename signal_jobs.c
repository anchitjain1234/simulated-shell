#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

// To keep track of length ofprocesses data structure.
int pid_counter = -1;

pid_t globalpid;
pid_t globalpgid;

typedef struct pidst{
	pid_t pid;
	int dead;
	/*
	dead = 0 Process running in background
	dead = 1 Process has died
	dead = 2 Process stopped in background
	dead = -1 Process is in foreground
	*/
}pidstore;

pidstore processes[100];

//SIGQUIT handler
void quithandler(int sig)
{
	printf("Killing all processes\n");
	int i;
	for(i=0;i<=pid_counter;i++)
	{
		if(processes[i].dead!=1)
	  		{
	  			printf("Killing %d\n",processes[i].pid);
	  			kill(processes[i].pid,SIGKILL);
	  		}
	}
	printf("Type quit to quit program.\n");
}

//SIGINT handler
void inthandler(int sig)
{
	printf("yo from int\n");
	int i;

	for(i=0;i<=pid_counter;i++)
	  	{
	  		if(processes[i].dead==-1)
	  		{
	  			kill(processes[i].pid,SIGKILL);
	  			break;
	  		}
	  	}
}

//SIGTSTP handler
void tstphandler(int sig)
{
	int i;

	for(i=0;i<=pid_counter;i++)
	  	{
	  		if(processes[i].dead==-1)
	  		{
	  			processes[i].dead=2;
	  			kill(processes[i].pid, SIGSTOP);
	  			break;
	  		}
	  	}
	  	tcsetpgrp(STDOUT_FILENO,globalpgid);
		tcsetpgrp(STDIN_FILENO,globalpgid);
}

//SIGCHLD handler
void sigchldhandler(int sig)
{
	int i;
  	for(i=0;i<=pid_counter;i++)
	{
		if(processes[i].dead!=1)
		{
			int status;
			int wst=waitpid(processes[i].pid,&status,WNOHANG);
			if(wst!=0)
			{
				processes[i].dead=1;
			}
		}
	}
}

//To check if ampersand is present in back of entered command or not.
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

//To check first space in entered command to seperate arguments from main command.
int cmd(char *command)
{
	int i;
	for (i = 0; i < strlen(command); ++i)
	{
		if(command[i]==' ')
		{
			return i+1;
		}
	}
	return strlen(command)+1;
}

int main(int argc, char const *argv[])
{
	globalpid=getpid();
	setpgid(globalpid,globalpid);
	globalpgid=getpgid(globalpid);
	setsid();
	signal(SIGTTOU, SIG_IGN);
	signal(SIGCHLD, sigchldhandler);
	signal(SIGQUIT,quithandler);
	while(1)
	{
		signal(SIGTSTP, SIG_IGN);
		signal(SIGINT,SIG_IGN);
		char command[100];
		int i,status;
		pid_t pid;

		printf("Enter command :");
		fgets(command,sizeof command,stdin);

		char *pos;
		if ((pos=strchr(command, '\n')) != NULL)
    		*pos = '\0';

    	if(strcmp(command,"jobs")==0)
    	{
    		for(i=0;i<=pid_counter;i++)
    		{
    			if(processes[i].dead!=1)
    			{
    				int status;
    				int wst=waitpid(processes[i].pid,&status,WNOHANG);
    				printf("wst =%d for pid =%d\n", wst,processes[i].pid);
    				if(wst!=0)
    				{
    					processes[i].dead=1;
    				}
    				else
    				{
    					if(processes[i].dead == 0)
    					{
    						printf("%d  Running\n", processes[i].pid);
    					}
    					else if(processes[i].dead == 2)
    					{
    						printf("%d  Stopped\n", processes[i].pid);
    					}
    				}
    				
    			}
    		}
    		continue;
    	}
    	else if(strlen(command)>6 && strncmp(command,"start",5) == 0)
    	{
    		char *pid_entered=(char *)malloc(sizeof(char)*(strlen(command)-6));
    		int i;
    		for(i=0;i<=strlen(command)-6;i++)
    		{
    			pid_entered[i]=command[6+i];
    		}
    		int pidint=atoi(pid_entered);
    		for(i=0;i<=pid_counter;i++)
			{
				if(processes[i].pid==pidint)
				{
					processes[i].dead = 0;
				}
			}
    		kill(pidint,SIGCONT);
    	}
    	else if(strlen(command)>5 && strncmp(command,"stop",4) == 0)
    	{
    		char *pid_entered=(char *)malloc(sizeof(char)*(strlen(command)-5));
    		int i;
    		for(i=0;i<=strlen(command)-5;i++)
    		{
    			pid_entered[i]=command[5+i];
    		}
    		int pidint=atoi(pid_entered);
    		for(i=0;i<=pid_counter;i++)
			{
				if(processes[i].pid==pidint)
				{
					processes[i].dead = 2;
				}
			}
    		kill(pidint,SIGSTOP);
    	}
    	else if(strcmp(command,"quit")==0)
    	{
    		exit(0);
    	}
    	else if(strlen(command)>0 && command!=" ")
    	{
    		int check = checkampersand(command);
			int tok=cmd(command),lentok=strlen(command)-tok;
			pid_counter+=1;
			if(check)
			{
				lentok-=2;
			}

			char *token=(char *)malloc(tok*sizeof(char));
			for(i=0;i<tok-1;i++)
			{
				token[i]=command[i];
			}
			char *args=(char *)malloc(sizeof(char)*lentok);
			for(i=0;i<lentok;i++)
			{
				args[i]=command[tok+i];
			}
			// char *pathini="/bin/";
			// char *pathfin=malloc(sizeof(pathini)+sizeof(token)+1);
			// strcpy(pathfin,pathini);
			// strcat(pathfin,token);
			int execer;
			pid=fork();
			if(pid==-1)
			{
				perror("Error in forking");
			}
			else if(pid == 0)
			{
				setpgid(getpid(),getpid());
				if(!check)
				{
					tcsetpgrp(STDOUT_FILENO,getpgid(getpid()));
					tcsetpgrp(STDIN_FILENO,getpgid(getpid()));
				}
				// Signal handlers are not working they are being replaced by their default action.
				signal(SIGTSTP, tstphandler);
				signal(SIGINT,inthandler);
				if(strlen(token)>0)
				{
					execer=execlp(token,token,args,(char*)NULL);
					if (execer==-1)
					{
						printf("Error in execution.Command = %s and args =%s\n",token,args );
					}
					exit(0);
				}
				
			}
			else
			{
				if(check)
				{
					processes[pid_counter].pid=pid;
					processes[pid_counter].dead=0;
				}
				if(!check && strlen(token)>0)
				{
					processes[pid_counter].pid=pid;
					processes[pid_counter].dead=-1;
					waitpid(pid,&status,0);
					tcsetpgrp(STDOUT_FILENO,globalpgid);
					tcsetpgrp(STDIN_FILENO,globalpgid);
					processes[pid_counter].dead=1;
				}
				continue;
			}
    	}
		
	}
	return 0;
}