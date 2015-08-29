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
	}
	return 0;
}

int main(int argc, char const *argv[])
{
	while(1)
	{
		char temp[100];
		printf("Enter command : ");
		fgets(temp,sizeof temp,stdin);

		char *pos;
		if ((pos=strchr(temp, '\n')) != NULL)
    		*pos = '\0';

		int check=checkampersand(temp);
		if(check)
		{
			char *redirect = ">/dev/null 2>&1";
			strcat(temp,redirect);
		}
		system(temp);
	}
	return 0;
}