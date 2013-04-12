#include "headers/type.h"

////////////////////////////////////////////////////////
//
//	Command Functions
//
////////////////////////////////////////////////////////


int _menu()
{
	printf("~~~~~~~MENU~~~~~~~\n\n");
	
	
	
	return 0;
}

int _ls(char *path)
{
	MINODE *mp = proc[0].cwd;
//	int isRoot = -1;
	unsigned long ino;
	char dirName[64];
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
	
///////////////////////////////////////////////////////////////////////////////////////////////
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
///////////////////////////////////////////////////////////////////////////////////////////////
	
	return 0;
}

int _cd(char *pathname)
{
	printf("~~~~~~~~CD~~~~~~~~\n\n");
	int result = -1;
  unsigned long ino, pino;
  MINODE * mip;

  // No pathname, go to root
 

  if(pathname[0] == 0){ 
    iput(running->cwd);
    running->cwd = iget(ROOT_DEV, ROOT_INODE);

    result = 0;
  }else if(strcmp(pathname, ".")==0){
    // Stay here
    running->cwd = running->cwd;
  }else if(strcmp(pathname, "..")==0){
   
    // We need to find the inode of the parent
    printDir(running->cwd->ino);
    findino(running->cwd, &ino, &pino);
   // printf("ino = %d\npino = %d\n\n", ino, pino);

    mip = iget(dev, pino);
    iput(running->cwd);
    running->cwd = mip;

  }else{

  // Get the inode of the pathname into MINODE
    ino = getino(&dev, pathname);
    printf("CD -- ino = %d\n", (int)ino);
    mip = iget(dev, ino);

    // Make sure its a DIR
    if(mip->INODE.i_mode == DIR_MODE){
        printf("BEFORE CD:\n");
        printInode(&running->cwd->INODE);
        iput(running->cwd);
        running->cwd = mip;

        printf("AFTER CD:\n");
        printInode(&running->cwd->INODE);

        result = 0;
    }else{
        result = 1;
        printf("Given pathname is not a directory...\n");
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

		// Get the MINODE of the parent
		pip = iget(dev, ino);
		printInode(&pip->INODE);

		printf("i_mode: %x\nDIR_MODE: %x\n", pip->INODE.i_mode, DIR_MODE);
		if(pip->INODE.i_mode == DIR_MODE){
			printf("THIS IS A DIRECTORY!\n");
		}else{
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

int _pwd()
{
	printf("~~~~~~~PWD~~~~~~~~\n\n");
	
  if(running->cwd->ino == ROOT_INODE){
    printf("/");
  }else{
	rec_pwd(running->cwd);
  }
	printf("\n");
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
int _stat(char *pathname){

	/*
	1. Get inode of pathname into an MINODE:
         ino = getino(&dev, pathname); // Use YOUR showBlock program; ino > 0
         mip = iget(dev, ino);         // iget() returns a pointer to minode[]
         
  3. Copy entries of INODE into stat struct;

     As you can see, everything needed by the stat struct is in minode.INODE
     Write C statements to copy from mip->INODE to stPtr->fields.

       struct ext2_inode {
	i_mode;		/* File mode 
	i_uid;		/* Owner Uid
	i_size;		/* Size in bytes 
	i_atime;	/* Access time 
	i_ctime;	/* Creation time 
	i_mtime;	/* Modification time 
	i_dtime;	/* Deletion Time 
	i_gid;		/* Group Id 
	i_links_count;	/* Links count 
	i_blocks;	/* Blocks count 
        .................................
        i_block[15];    // Allocated disk blocks 
      }

	 

      struct stat {
        st_dev;      // dev
        st_ino;      // ino

        st_mode;     // i_mode
        st_nlink;    // i_links_count
        st_uid;      // i_uid
        st_gid;      // i_gid
        st_rdev;     // IGNORE THIS
        st_size;     // i_size
        st_blksize;  // 1024
        st_blocks;   // i_blocks
        st_atime;    // i_atime
        st_mtime;    // i_mtime
        st_ctime;    // i_ctime
      };

  4. Print the entries of the stat struct;
  5. iput(mip);      // dispose of minode; see NOTE below.
     return 0 for success;
     */

     struct stat mystat;
     unsigned long ino;
     MINODE * mip;
   

  printf("before:%s\n", pathname);

     ino = getino(&dev, pathname);
      printf("middle:%s\n", pathname);
     mip = iget(dev, ino);

	  mystat.st_dev = dev;
	  mystat.st_ino = ino;
	  mystat.st_mode = mip->INODE.i_mode;
	  mystat.st_nlink = mip->INODE.i_links_count;
	  mystat.st_uid = mip->INODE.i_uid;
	  mystat.st_gid = mip->INODE.i_gid;
	  mystat.st_size = mip->INODE.i_size;
	  mystat.st_blksize = BLOCK_SIZE;
	  mystat.st_blocks = mip->INODE.i_blocks;
	  mystat.st_atime = mip->INODE.i_atime;
	  mystat.st_ctime = mip->INODE.i_ctime;
	  mystat.st_mtime = mip->INODE.i_mtime;

	  printf("after:%s\n", pathname);
	  printf("---- stat - %s ----\n\n", basename(pathname));
	  printf("Device: %d\n", mystat.st_dev);
	  printf("Inode: %d\n", mystat.st_ino);
	  printf("Permissions: ");

	  printf( (mystat.st_mode & S_IRUSR) ? "r" : "-");
      printf( (mystat.st_mode & S_IWUSR) ? "w" : "-");
      printf( (mystat.st_mode & S_IXUSR) ? "x" : "-");
      printf( (mystat.st_mode & S_IRGRP) ? "r" : "-");
      printf( (mystat.st_mode & S_IWGRP) ? "w" : "-");
      printf( (mystat.st_mode & S_IXGRP) ? "x" : "-");
      printf( (mystat.st_mode & S_IROTH) ? "r" : "-");
      printf( (mystat.st_mode & S_IWOTH) ? "w" : "-");
      printf( (mystat.st_mode & S_IXOTH) ? "x" : "-");
      printf("\n");

      printf("Links: %d\n", mystat.st_nlink);
      printf("UID: %d\n", mystat.st_uid);
      printf("GID: %d\n", mystat.st_gid);
      printf("Size: %d\n", mystat.st_size);
      printf("Blocksize: %d\n", mystat.st_blksize);
      printf("# of Blocks: %d\n", mystat.st_blocks);
      printf("aTime: %s", ctime(&mystat.st_atime));
      printf("cTime: %s", ctime(&mystat.st_ctime));
      printf("mTime: %s", ctime(&mystat.st_mtime));
      printf("\n");

      iput(mip);
	return 0;
}
int __exit()
{
	printf("~~~~~QUITTING~~~~~\n\n");
	quit();
	exit(0);
	return 0;
}

int rec_pwd(MINODE *wd){

  /*
  Write this as a recursive function, which

   1. if wd is already the root:
         print /; return;

   2. Get parent's MINODE pointer wd; 
          (HOW? get i_block[0]; then iget(dev, ino of ..))
      Call pwd(wd) again with parent's MINODE pointer;

   3. Print wd's name followed by a /;
       (HOW TO get the name string of a MINODE?)
       You have this guy's ino and its parent's MINODE.
       Search the parent DIR for an entry with this ino. Then you have its name.

   4. FOR LEVEL-3: If you implement MOUNTing, make sure your recursion can
                   cross mounting points.
  */

  unsigned long pino = 0, ino = wd->ino;
  MINODE * nwd;
  char buf[BLOCK_SIZE];
  char name[256];
  int c = 0;

  if(wd->ino == ROOT_INODE){
      //printf("");
  }else{

      get_block(dev, wd->INODE.i_block[0], buf);
      // Iterate through dir entries to find ..
      dp = (DIR *)buf;
      cp = buf; 
  

      while(cp < buf + BLOCK_SIZE){

        if(c==1){
          pino = dp->inode;
          printf("FOUND PARENT - %d\n", pino);
        }
        cp += dp->rec_len;            /* advance by rec_len */
        dp = (DIR *)cp;
        c++;
      }

      if(pino == 0){
        printf("Error finding '..'\n");
        return -1;
      }else{

       // getchar();
        //printDir(pino);
     
        nwd = iget(dev, pino);
      
        rec_pwd(nwd);
     
        findmyname(nwd, ino, &name[0]);

        printf("/%s", name);
      }

  }

  return 0;

}

int rec_complete(MINODE *wd){

  /*
  Write this as a recursive function, which

   1. if wd is already the root:
         print /; return;

   2. Get parent's MINODE pointer wd; 
          (HOW? get i_block[0]; then iget(dev, ino of ..))
      Call pwd(wd) again with parent's MINODE pointer;

   3. Print wd's name followed by a /;
       (HOW TO get the name string of a MINODE?)
       You have this guy's ino and its parent's MINODE.
       Search the parent DIR for an entry with this ino. Then you have its name.

   4. FOR LEVEL-3: If you implement MOUNTing, make sure your recursion can
                   cross mounting points.
  */

  unsigned long pino = 0, ino = wd->ino;
  MINODE * nwd;
  char buf[BLOCK_SIZE];
  char name[256];
  int c = 0;

  if(wd->ino == ROOT_INODE){
 		
  }else{

      get_block(dev, wd->INODE.i_block[0], buf);
      // Iterate through dir entries to find ..
      dp = (DIR *)buf;
      cp = buf; 
  

      while(cp < buf + BLOCK_SIZE){

        if(c==1){
          pino = dp->inode;
        }

        cp += dp->rec_len;            /* advance by rec_len */
        dp = (DIR *)cp;
        c++;
      }

      if(pino == 0){
        printf("Error finding '..'\n");
        return -1;
      }else{

       // getchar();
        //printDir(pino);
     
        nwd = iget(dev, pino);
      
        rec_pwd(nwd);
     
        findmyname(nwd, ino, &name[0]);

       

        strcat(completePath, "/");
        strcat(completePath, name);
       
      }

  }

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

	char str[64];

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
=======
  //**********************************************************************
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
  printf("end: %d\n", (int)buf + BLOCK_SIZE);

  while(cp < buf + BLOCK_SIZE  && dp->rec_len != 0){
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
  if(dp->rec_len - ideal_len >= need_len){

  	new_len = dp->rec_len - ideal_len;

  	dp->rec_len = ideal_len; // Set the last entry to the ideal length

  	cp += dp->rec_len;	// Move to the end of the last entry

  	dp = (DIR *)cp;

  	dp->inode = inumber;		/* Inode number */
    strncpy(dp->name, name, strlen(name));    /* File name */
    dp->name_len = strlen(name);		/* Name length */
    dp->rec_len = new_len;		/* Directory entry length */

  }else{ // We need to allocate a new block to store this entry
  		printf("HAHA");
  }


  dp = (DIR *)buf;
  cp = buf; 
  printf("end: %u\n", (int)buf + BLOCK_SIZE);

  while(cp < buf + BLOCK_SIZE  && dp->rec_len != 0){
  	prev = cp;
  	strncpy(str, dp->name, dp->name_len);
  	str[dp->name_len] = '\0';

  	printf("name: %s\n", str);
  	printf("rec_len: %d\n", dp->rec_len);
  	printf("name_len: %d\n", dp->name_len);
  	printf("ino: %d\n\n", dp->inode);
  	cp += dp->rec_len;            /* advance by rec_len */
  	dp = (DIR *)cp;
  }




  	put_block(1, pip->INODE.i_block[0], buf);

    time(&curTime);
    pip->INODE.i_atime = curTime;
    iput(pip);

return 1;
}
