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
	MINODE *mp = proc[0].cwd;
//	int isRoot = -1;
	unsigned long ino;
////////////////////////////////////////////////////////////////////////
	int i;
////////////////////////////////////////////////////////////////////////
	
	printf("~~~~~~~~LS~~~~~~~~\n\n");
	
	if (strcmp(path, "") == 0)
	{
		printf("From CWD:\n");
	}
	else
	{
		printf("Relative to %s\n", path);
		/*if (strncmp(path, "/", 1) == 0)
		{
			isRoot = 1;
		}
		else
			isRoot = 0;*/
			
		// set up the directory to be accessed
		ino = getino((int *)&(mp->dev), path);
//		printf("ino = %lu\n", ino);
		mp = iget(mp->dev, ino);
	}
	
	// look in the directory, iget(): load the inode into memory (it becomes an minode and increases its refcount by 1)
	// print out files
	// iput(): unload the minode
	
	// mp points at minode; 
	// Each data block of mp->INODE contains DIR entries
	// print the name strings of the DIR entries
	
///////////////////////////////////////////////////////////////////////////////////////////////
	for(i = 0; i < 12; i++)
	{
		if(mp->INODE.i_block[i] == 0)
			break;
//		printf("%d\n", inodePtr->i_block[i]);
		get_block(i);
		dp = (DIR *)block;
//		outcome = stat(/*filen*/, buf);
//		if (outcome != -1)
//		{
		getchar();
			if ( (dp->file_type & 0040000) == 0040000) // directory
			{
				printf("directoryJVJVJVJVJVJVJVVJVJ\n");
				getchar();
				/*dp = opendir(filen);
				if (dp == NULL)
					printf("Failed opening directory.\n");
				ep = readdir(dp);
				ep = readdir(dp);
				while ((ep = readdir(dp)) != NULL)
					printf ("\t %s", ep->d_name);
				printf("\n\n");
				closedir (dp);
				return;*/
			}
			/*else if ( (buf->st_mode & 0100000) == 0100000) // regular file
			{
				if ( (buf->st_mode & 0120000) == 0120000) // symbolic link
				{
					lstat(filen, buf);
					value = readlink(filen, file, 64);
					if ( value != -1)
					{
						file[value] = '\0';
						printf("file = %s\n", file);
					}
					else
						printf("made it to else2\n");
					return;
				}
			}
			
			printf("\n File_type\t Permissions\t Uid\t Size\t Creation Time  \n");
			printf("-----------\t-------------\t-----\t------\t---------------\n");
			printf("  Reg\t\t");
			
			if ( (buf->st_mode & (1 << 8)) ) // Owner can r
				printf("r");
			else
				printf("-");
			if ( (buf->st_mode & (1 << 7)) ) // Owner can w
				printf("w");
			else
				printf("-");
			if ( (buf->st_mode & (1 << 6)) ) // Owner can x
				printf("x");
			else
				printf("-");
				
			if ( (buf->st_mode & (1 << 5)) ) // Group can r
				printf("r");
			else
				printf("-");
			if ( (buf->st_mode & (1 << 4)) ) // Group can w
				printf("w");
			else
				printf("-");
			if ( (buf->st_mode & (1 << 3)) ) // Group can x
				printf("x");
			else
				printf("-");
			
			if ( (buf->st_mode & (1 << 2)) ) // Public can r
				printf("r");
			else
				printf("-");
			if ( (buf->st_mode & (1 << 1)) ) // Public can w
				printf("w");
			else
				printf("-");
			if ( (buf->st_mode & (1 << 0)) ) // Public can x
				printf("x");
			else
				printf("-");
			printf("\t%d", buf->st_uid);
			printf("\t%lu", buf->st_size);
			printf("\t%s\n", (ctime(&(buf->st_ctime))));*/
//		}
//		else
//			printf("stat() returned with an error.\n");
	}
///////////////////////////////////////////////////////////////////////////////////////////////
	
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
