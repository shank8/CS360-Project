#include "headers/type.h"



int main(int argc, char * argv[], char * env[])
{
	char line[128], command[128], pathname[128];
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
		strcpy(completePath, "");

		printf("\n\ninput a command (type help for more info): ");
		//read a line containting  command [pathname]; // [ ] means optional
		fgets(line, 256, stdin);
		line[strlen(line)-1] = '\0';

		//Find the command string and call the corresponding function;
		parseString(line, command, pathname);

		compPath(pathname);
		
		
		printf("PATHNAME: %s\n", pathname);
		ID = findCommand(command);
		switch(ID)
		{
			case -1 : printDir(running->cwd->ino);	break;
			case  0 : _menu  (arg1, pathname);	break;
			case  1 : _ls    (arg1, pathname);	break;
			case  2 : _cd    (arg1, pathname);	break;
			case  3 : _mkdir (arg1, pathname);	break;
			case  4 : _rmdir (arg1, pathname);	break;
			case  5 : _pwd   (arg1, pathname);	break;
			case  6 : _creat0(arg1, pathname);	break;
			case  7 : _rm    (arg1, pathname);	break;
			case  8 : _stat  (arg1, pathname);	break;
			case  9 : compPath(arg1); _link(arg1, pathname); break;
			case  10: _unlink(arg1, pathname); break;
			case  11: compPath(arg1); _symlink(arg1, pathname); break;
			case  12: _touch (arg1, pathname);	break;
			case  13: _chmod (arg1, pathname);	break;
			case  14: _chown (arg1, pathname);	break;
			case  15: _chgrp (arg1, pathname);	break;
			case  16: __exit (arg1, pathname);	break;
		}
	}
	
	quit();
	return 0;
}

