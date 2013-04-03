#include "headers/type.h"

////////////////////////////////////////////////////////
//
//	Command Functions
//
////////////////////////////////////////////////////////

int _menu(char *pathname)
{
	printf("~~~~~~~MENU~~~~~~~\n\n");
	
	
	
	return 0;
}

int _ls(char *path)
{
	int isRoot = -1;
	printf("~~~~~~~~LS~~~~~~~~\n\n");
	
	if (strcmp(path, "") == 0)
	{
		printf("From CWD:\n");
	}
	else
	{
		printf("Relative to %s\n", path);
		if (strncmp(path, "/", 1) == 0)
		{
			isRoot = 1;
		}
		else
			isRoot = 0;
		// set up the directory to be accessed
	}
	
	// look in the directory, iget(): load the inode into memory (it becomes an minode and increases its refcount by 1)
	// print out files
	// iput(): unload the minode
	
	return 0;
}

int _cd(char *pathname)
{
	printf("~~~~~~~~CD~~~~~~~~\n\n");
	
	
	
	return 0;
}

int _mkdir(char *pathname)
{
	printf("~~~~~~MKDIR~~~~~~~\n\n");
	
	
	
	return 0;
}

int _rmdir(char *pathname)
{
	printf("~~~~~~RMDIR~~~~~~~\n\n");
	
	
	
	return 0;
}

int _pwd(char *pathname)
{
	printf("~~~~~~~PWD~~~~~~~~\n\n");
	
	
	
	return 0;
}

int _creat0(char *pathname)
{
	printf("~~~~~~CREAT~~~~~~~\n\n");
	
	
	
	return 0;
}

int _rm(char *pathname)
{
	printf("~~~~~~~~RM~~~~~~~~\n\n");
	
	
	
	return 0;
}

int __exit(char *pathname)
{
	printf("~~~~~QUITTING~~~~~\n\n");
	quit();
	exit(0);
	return 0;
}
