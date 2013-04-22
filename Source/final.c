#include "headers/type.h"



int main(int argc, char * argv[], char * env[])
{
	char line[128], command[128], pathname[128], arg1[128];
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
		strcpy(arg1, "");
		strcpy(name[0], "");
		strcpy(completePath, "");
		printf("\n\ninput a command (type help for more info): ");
		//read a line containting  command [pathname]; // [ ] means optional
		fgets(line, 256, stdin);
		line[strlen(line)-1] = '\0';

		//Find the command string and call the corresponding function;
		parseString(line, arg1, command, pathname);
		
//printf("pathname = %s\n", pathname);
		
		compPath(pathname);
		strcpy(pathname, completePath);
		printf("PATHNAME: %s\n", pathname);
		ID = findCommand(command);
		switch(ID)
		{
			case -1 :							break;
			case  0 : _menu  (arg1, pathname);	break;
			case  1 : _ls    (arg1, pathname);	break;
			case  2 : _cd    (arg1, pathname);	break;
			case  3 : _mkdir (arg1, pathname);	break;
			case  4 : _rmdir (arg1, pathname);	break;
			case  5 : _pwd   (arg1, pathname);	break;
			case  6 : _creat0(arg1, pathname);	break;
			case  7 : _rm    (arg1, pathname);	break;
			case  8 : _stat  (arg1, pathname);	break;
			case  9 : _touch (arg1, pathname);	break;
			case  10: _chmod (arg1, pathname);	break;
			case  11: _chown (arg1, pathname);	break;
			case  12: _chgrp (arg1, pathname);	break;
			case  13: __exit (arg1, pathname);	break;
		}
	}
	
	quit();
	return 0;
}

