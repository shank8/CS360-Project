#include "headers/type.h"

////////////////////////////////////////////////////////
//
//	Command Functions
//
////////////////////////////////////////////////////////
extern int dev;

int _menu(char *pathname)
{
	printf("~~~~~~~MENU~~~~~~~\n\n");
	
	
	
	return 0;
}

int _ls(char *pathname)
{
	printf("~~~~~~~~LS~~~~~~~~\n\n");
	
	
	
	return 0;
}

int _cd(char *pathname)
{
	printf("~~~~~~~~CD~~~~~~~~\n\n");
	
	
	
	return 0;
}

int _mkdir(char *pathname)
{
	char * parent, * child;
	char fullpath[128], fullpath2[128];

	unsigned long ino;
	MINODE * pip;

	// Create a copy of the pathname so we don't alter it directly
	strcpy(fullpath, pathname);
	strcpy(fullpath2, pathname);

	printf("~~~~~~MKDIR~~~~~~~\n\n");
	/*  1. Ask for a pahtname, e.g. /a/b/c  or a/b/c, etc.

		2.  if (pathname[0] == '/') 
		        dev = root->dev;
		     else
		        dev = cwd->dev;
		3. Let  
		     parent = dirname(pathname);   parent= "/a/b"
		     child  = basename(pathname);  child = "c"

		4. Get the In_MEMORY minode of parent:

		         ino  = getino(&dev, parent);
		         pip  = iget(dev, ino); 

		   Then, verify :
		         parent INODE is a DIR AND
		         child des NOT exists in the parent directory;
		               
		5. Call 
		          r = my_mkdir(pip, child);
		 
		6.  return(r);*/

		if(pathname[0] == '/'){
			//dev = root->dev;
		}else{
			//dev = cwd->dev;
		}
		printf("fullPath: %s\n", fullpath);
		parent = dirname(fullpath); 
		child  = basename(fullpath2);

		printf("parent: %s\n", parent);
		printf("child: %s\n", child);

		ino = getino(&dev, parent);
		pip = iget(dev, ino);

		if(S_ISDIR(pip->INODE.i_mode)){
			printf("THIS IS A DIRECTORY!\n");
		}else{
			printf("THIS IS NOT A DIRECTORY\n");
		}

	
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
