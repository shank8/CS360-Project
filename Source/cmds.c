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
	struct stat mystat;
	int r;
	MINODE *mp = proc[0].cwd;
	unsigned long ino;
	char dirName[64];
	int i;
	
	r = do_stat(path, &mystat);
	
	printf("~~~~~~~~LS~~~~~~~~\n\n");
	
	if (strcmp(path, "") == 0)
	{
		printf("From CWD:\n");
	}
	else
	{
		printf("Relative to %s\n", path);
			
		// set up the directory to be accessed
		ino = getino((int *)&(mp->dev), path);
		if (ino == 0)
		{
			printf("Could Not Find Path: ls()\n");
			return -1;
		}
//		printf("ino = %lu\n", ino);
		mp = iget(mp->dev, ino);
	}
	
	// look in the directory, iget(): load the inode into memory (it becomes an minode and increases its refcount by 1)
	// print out files
	// iput(): unload the minode
	
	// mp points at minode; 
	// Each data block of mp->INODE contains DIR entries
	// print the name strings of the DIR entries

	for(i = 0; i < 12; i++)
	{
		lseek(fd, mp->INODE.i_block[i] * BLOCK_SIZE, 0);
		read(fd, datablock, BLOCK_SIZE);
		
		dp = (DIR *)datablock;
		cp = datablock;
		strcpy(dirName, dp->name);
		dirName[dp->name_len] = '\0';
		if (strncmp(dp->name, "", dp->name_len) != 0)
		{
			printf("Block[%d]:\n", i);
			printf("name length: %d\n", dp->name_len);
			printf("Directory name: %s\n", dirName);
		}
		
		while(cp < datablock + BLOCK_SIZE && dp->rec_len != 0)
		{
			cp += dp->rec_len;			// advance cp by rlen in bytes
			dp = (DIR *)cp;				// pull dp to the next record
			strcpy(dirName, dp->name);
			dirName[dp->name_len] = '\0';
			if (strncmp(dp->name, "", dp->name_len) != 0)
			{
				printf("name length: %d\n", dp->name_len);
				printf("Directory name: %s\n", dirName);
			}
        }
	}
	
	return 0;
}

int _cd(char *pathname)
{
	printf("~~~~~~~~CD~~~~~~~~\n\n");
	int result = -1;
	unsigned long ino;
	MINODE * mip;

	// No pathname, go to root
	if(pathname[0] == 0)
	{ 
		iput(running->cwd);
		running->cwd = iget(ROOT_DEV, ROOT_INODE);

		result = 0;
	}
	else
	{
		// Get the inode of the pathname into MINODE
		ino = getino(&dev, pathname);
		mip = iget(dev, ino);

		// Make sure its a DIR
		if(mip->INODE.i_mode == DIR_MODE)
		{

		}
		else
		{
		  
		}
	}

	return result;
}
  
int _mkdir(char *pathname)
{
	char * parent, * child;
	char fullpath[128], fullpath2[128];
	int r;
	unsigned long ino;
	MINODE * pip;

	// Create a copy of the pathname so we don't alter it directly
	strcpy(fullpath, pathname);
	strcpy(fullpath2, pathname);

	printf("~~~~~~MKDIR~~~~~~~\n\n");
	/*  1. Ask for a pahtname, e.g. /a/b/c  or a/b/c, etc.

	2.  if (pathname[0] == '/') 
		  dev = root->dev;
	   else)
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

	if(pathname[0] == '/')
	{
		//dev = root->dev;
	}
	else
	{
		//dev = cwd->dev;
	}

	printf("fullPath: %s\n", fullpath);
	parent = dirname(fullpath); 
	child  = basename(fullpath2);

	printf("parent: %s\n", parent);
	printf("child: %s\n", child);

	ino = getino(&dev, parent);

	// Get the MINODE of the parent
	pip = iget(dev, ino);
	printInode(&pip->INODE);

	printf("i_mode: %x\nDIR_MODE: %x\n", pip->INODE.i_mode, DIR_MODE);
	if(pip->INODE.i_mode == DIR_MODE)
	{
		printf("THIS IS A DIRECTORY!\n");
	}
	else
	{
		printf("THIS IS NOT A DIRECTORY\n");
	}

	r = my_mkdir(pip, child);

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





int my_mkdir(MINODE *pip, char *name)
{
	/*
		1. pip points at the parent minode[] of "/a/b", name is a string "c") 

		2. allocate an inode and a disk block for the new directory;
		       call   inumber=ialloc(dev),      bnumber=balloc(dev)
		   where dev=pip->dev, i.e. SAME dev as the parent directory.

		3. call mip = iget(dev,inumber) to load the inode into a minode[] (in order to
		   wirte contents into the intended INODE in memory).

		4. Write contents into mip->INODE, including:
		        (same as the root directory in mkfs)
		        INOODE.i_block[0] = bnumber; 
		        all other INODE.i_block[] = 0;
		        mip->dirty = 1;  // mark dirty 

		//5. call  iput(mip);  
		//   which should write the new INODE out to disk. 
   

  // C CODE:
  */

	pip->dirty = 1;
	int inumber;
	int bnumber;
	int ideal_len;
	int need_len;
	int new_len;
	int i;
	DIR * dp;
	char * cp, * prev;
	char buf[BLOCK_SIZE];
	MINODE * mip;
	time_t curTime;

//	char str[64];

	dev = pip->dev;

	inumber = ialloc(dev);
	bnumber = balloc(dev);
/*
typedef struct Minode
{		
	INODE    INODE;               // disk inode
	ushort   dev;
	unsigned long ino;
	ushort   refCount;
	ushort   dirty;
	ushort   mounted;
	struct Mount *mountptr;
	char     name[128];           // name string of file
} MINODE;
*/
	mip = iget(dev,inumber);

	mip->INODE.i_mode = DIR_MODE;		/* DIR and permissions */
	mip->INODE.i_uid  = running->uid;	/* Owner Uid */
	mip->INODE.i_gid  = running->gid;	/* Group Id */
	mip->INODE.i_size = BLOCK_SIZE;		/* Size in bytes */

	mip->INODE.i_links_count = 2;	/* Links count */

	mip->INODE.i_atime=mip->INODE.i_ctime=mip->INODE.i_mtime = time(0L); 

	mip->INODE.i_blocks = 2;     	/* Blocks count in 512-byte blocks */
	mip->dirty = 1;               /* mark dirty */

	for (i=0; i<15; i++)
	{
		mip->INODE.i_block[i] = 0;
	}

	mip->INODE.i_block[0] = bnumber; 

	printInode(&mip->INODE);

	iput(mip);
  /*


6. Write the . and .. entries into a buf[ ] of BLOCK_SIZE; 
   write buf[] to the disk block allocated to this directory;

// C CODE:
*/
	dp = (DIR *)buf;

	dp->inode = inumber;			/* Inode number */
	strncpy(dp->name, ".", 1);		/* File name */
	dp->name_len = 1;				/* Name length */
	dp->rec_len = 12;				/* Directory entry length */

	cp = buf; 
	cp += dp->rec_len;				/* advance by rec_len */
	dp = (DIR *)cp;

	dp->inode = pip->ino;			/* Inode number of parent DIR */
	dp->name_len = 2;				/* Name length */
	strncpy(dp->name, "..", 2);		/* File name */
	dp->rec_len = BLOCK_SIZE - 12;	/* last DIR entry length to end of block */

	put_block(dev, bnumber, buf);

/*
7. Finally, enter name into parent's directory 
   Read parent's data block into buf[];

     !!! LISTEN TO LECTURE CAREFULLY ON HOW TO DO THESE !!!:

(1). EXT2 DIR entries: Each DIR entry has rec_len and name_len.
(2). The rec_len of the LAST entry in a data block is to the end of the block.

      --|-4---2----2--|----|---------|--------- rlen ->------------------------|
        |ino rlen nlen NAME|.........|ino rlen nlen|NAME                       |
      --------------------------------------------------------------------------

(3). When entering a new entry with name_len = n:
          need_length = 4*((8 + name_len + 3)/4)  //a multiple of 4

(4). Step to the last entry in a data block. Its IDEAL_LENGTH is 
          IDEAL_LENGTH = 4*((8+name_len+3)/4).

     if (rec_len - IDEAL_LENGTH >= need_length){
          enter the new entry as the LAST entry and trim the previous entry
          to its IDEAL_LENGTH

      --|-4---2----2--|----|---------|--------- rlen ->------------------------|
        |ino rlen nlen NAME|.........|ino rlen nlen|NAME                       |
      --------------------------------------------------------------------------

      --|-4---2----2--|----|---------|-----ideal -------|--- rlen->------------|
        |ino rlen nlen NAME|.........|ino rlen nlen|NAME|ino rlen nlen xxx     |
      --------------------------------------------------------------------------
     }
     else{ // allocate a new data block; 
              enter the new entry as the first entry in the new data block
     }
        
8. Write parent's data block back to disk;

9. inc parent inodes's link count by 1; touch its atime and mark it DIRTY
*/
	get_block(dev, pip->INODE.i_block[0], buf);

	dp = (DIR *)buf;
	cp = buf; 
	printf("end: %s\n", buf + BLOCK_SIZE);

	while(cp < buf + BLOCK_SIZE  && dp->rec_len != 0)
	{
		prev = cp;  
		cp += dp->rec_len;            /* advance by rec_len */
		dp = (DIR *)cp;
	}
 
	dp = (DIR *)prev; // dp is the last entry
	cp = prev;

	need_len = 4*((8 + strlen(name) + 3)/4);  // Get needed length for new directory

	ideal_len = 4*((8 + dp->name_len + 3)/4);	  // Get ideal length of the last dir_entry
	printf("need: %d\n", need_len);
	printf("ideal: %d\n", ideal_len);
	printf("cur: %d\n\n", dp->rec_len);
	
	if(dp->rec_len - ideal_len >= need_len)
	{
		new_len = dp->rec_len - ideal_len;
		dp->rec_len = ideal_len; // Set the last entry to the ideal length
		cp += dp->rec_len;	// Move to the end of the last entry

		dp = (DIR *)cp;

		dp->inode = inumber;		/* Inode number */
		strncpy(dp->name, name, strlen(name));    /* File name */
		dp->name_len = strlen(name);		/* Name length */
		dp->rec_len = new_len;		/* Directory entry length */
	}
	else
	{ // We need to allocate a new block to store this entry
		printf("HAHA");
	}
	
	printInode(&pip->INODE);
	put_block(1, pip->INODE.i_block[0], buf);

	get_block(0, pip->INODE.i_block[0], buf);


  	put_block(1, pip->INODE.i_block[0], buf);


    time(&curTime);
    pip->INODE.i_atime = curTime;
    iput(pip);

	return 1;
}
