#include "headers/type.h"



int main(int argc, char * argv[], char * env[])
{
	char line[128], command[64], pathname[64];
	int ID;
	// DEVICE SELECT
	get_device();

	// INITIALIZE 
	init();
	
	// MOUNT ROOT
	mount_root();

	// PROCESS LOOP
	while(1)
	{
		strcpy(line, "");
		strcpy(command, "");
		strcpy(pathname, "");
		printf("input a command (type help for more info): ");
		//read a line containting  command [pathname]; // [ ] means optional
		fgets(line, 128, stdin);
		line[strlen(line)-1] = '\0';

		//Find the command string and call the corresponding function;
		parseString(line, command, pathname);
		ID = findCommand(command);
		switch(ID)
		{
			case -1 : 						break;
			case  0 : _menu("");			break;
			case  1 : _ls(pathname);		break;
			case  2 : _cd(pathname);		break;
			case  3 : _mkdir(pathname);		break;
			case  4 : _rmdir(pathname);		break;
			case  5 : _pwd("");				break;
			case  6 : _creat0(pathname);	break;
			case  7 : _rm(pathname);		break;
			case  8 : __exit("");			break;
		}
	}
	
	quit();
	return 0;
}

