#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

int pid_counter = -1;

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

void quithandler(int sig)
{
	printf("Type quit to quit program.\n");
}

void inthandler(int sig)
{
	// printf("INT caught");
	pid_t pid;
	int i;

	for(i=0;i<=pid_counter;i++)
	  	{
	  		// printf("pid = %d , dead = %d\n",processes[i].pid,processes[i].dead );
	  		if(processes[i].dead==-1)
	  		{
	  			kill(processes[i].pid,SIGINT);
	  			break;
	  		}
	  	}
}

void tstphandler(int sig)
{
	// printf("yo\n");
	pid_t pid;
	int i;

	for(i=0;i<=pid_counter;i++)
	  	{
	  		// printf("pid = %d , dead = %d\n",processes[i].pid,processes[i].dead );
	  		if(processes[i].dead==-1)
	  		{
	  			kill(processes[i].pid,SIGTSTP);
	  			break;
	  		}
	  	}
}

void sigchldhandler(int sig)
{
	pid_t pid;
	int i;
  	pid = wait(NULL);

  	if(pid != -1)
  	{
  		for(i=0;i<=pid_counter;i++)
	  	{
	  		if(processes[i].pid==pid)
	  		{
	  			processes[i].dead=1;
	  			break;
	  		}
	  	}
  	}

  	// printf("Pid %d exited.\n", pid);
}

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
	signal(SIGCHLD, sigchldhandler);
	signal(SIGTSTP, tstphandler);
	signal(SIGINT,inthandler);
	signal(SIGQUIT,quithandler);
	while(1)
	{
		// printf("pid=%d\n",getpid() );
		sigset_t	newmask, oldmask, pendmask;
		sigemptyset(&newmask);
		sigaddset(&newmask, SIGINT);
		sigaddset(&newmask, SIGQUIT);
		sigaddset(&newmask, SIGTSTP);
		sigprocmask(SIG_UNBLOCK, &newmask, NULL);
		char command[100];
		int i,j,status;
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
    					printf("%d\n", processes[i].pid);
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
    		kill(pidint,SIGSTOP);
    	}
    	else if(strcmp(command,"quit")==0)
    	{
    		exit(0);
    	}
    	else if(strlen(command)>0 && command!=" ")
    	{
    		int check = checkampersand(command),nooftokens=0;
			int tok=cmd(command),lentok=strlen(command)-tok;

			if(check)
			{
				pid_counter+=1;
				lentok-=2;
			}

			char *token=(char *)malloc(sizeof(char)*tok);
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
			pid=vfork();
			if(pid==-1)
			{
				perror("Error in forking");
			}
			else if(pid == 0)
			{
				sigprocmask(SIG_UNBLOCK, &newmask, NULL);
				setpgid(0, 0);
				// pid = getpid();
	  			// setpgid(pid, pid);
				// signal(SIGTSTP, tstphandler);
				// printf("pid=%d\n",getpid() );
				int execer=execlp(token,token,args,(char*)NULL);
				if (execer==-1)
					exit(0);
			}
			else
			{
				sigprocmask(SIG_UNBLOCK, &newmask, NULL);
				// setpgid(pid, pid);
				if(check)
				{
					processes[pid_counter].pid=pid;
					processes[pid_counter].dead=0;
				}
				if(!check)
				{
					processes[pid_counter].pid=pid;
					processes[pid_counter].dead=-1;
					waitpid(pid,&status,0);
					processes[pid_counter].dead=1;
				}
				continue;
				// continue;
			}
    	}
		
	}
	return 0;
}