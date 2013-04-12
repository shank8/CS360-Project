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
		printf("\n\ninput a command (type help for more info): ");
		//read a line containting  command [pathname]; // [ ] means optional
		fgets(line, 256, stdin);
		line[strlen(line)-1] = '\0';

		//Find the command string and call the corresponding function;
		parseString(line, command, pathname);

		compPath(pathname);
		strcpy(pathname, completePath);
		printf("PATHNAME: %s\n", pathname);
		ID = findCommand(command);
		switch(ID)
		{
			case -1 : 						break;
			case  0 : _menu();			    break;
			case  1 : _ls(pathname);		break;
			case  2 : _cd(pathname);		break;
			case  3 : _mkdir(pathname);		break;
			case  4 : _rmdir(pathname);		break;
			case  5 : _pwd();				break;
			case  6 : _creat0(pathname);	break;
			case  7 : _rm(pathname);		break;
			case  8 : _stat(pathname);		break;
			case  9 : __exit();				break;
		}
	}
	
	quit();
	return 0;
}

void compPath(char * path){

	strcpy(completePath, "");

	if(path[0] == '/'){
		strcpy(completePath, path);
		return;
	}else if(strcmp(path, ".")==0){
		return;
	}else if(strcmp(path, "..")==0){
		return;
	}else{
		//strcat(completePath, "/");

		if(running->cwd->ino == ROOT_INODE){
			strcat(completePath, "");
		}else{
			rec_complete(running->cwd);
		}
		strcat(completePath, "/");
		strcat(completePath, path);
		

	}
	printf("Complete Path: %s\n", completePath);

	return;

}