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
	INODE *inode;
	struct stat fileStat;
//	int isRoot = -1;
	unsigned long ino;
	char name[64];
	char createtime[64];
	time_t ntime;
	struct tm * mytm;
////////////////////////////////////////////////////////////////////////
	int i;
////////////////////////////////////////////////////////////////////////
	
	printf("~~~~~~~~LS~~~~~~~~\n\n");
		
	
		if(strcmp(path, "")==0){
			mp = running->cwd;
		}else{

			ino = getino(&(mp->dev), path);

			if (ino == 0)
			{
				printf("Could Not Find Path: ls()\n");
				return -1;
			}

			mp = iget(mp->dev, ino);
		}
		

	for(i = 0; i < 12; i++)
	{
		get_block(dev, mp->INODE.i_block[i], datablock);
		
		
		dp = (DIR *)datablock;
		cp = datablock;
		
		
		while(cp < datablock + BLOCK_SIZE && dp->rec_len != 0)
		{
			// Take each inode, get the actual INODE *
			inode = findInode((int)dp->inode);

			// Now copy the contents into a stat struct
			do_stat(&fileStat, inode, dp->inode);

		
			if(S_ISDIR(fileStat.st_mode))
			{
				// DIR - display files in the directory
				printf("d");
			
				
			}else{
				// Display general info only
				printf("-");
			
			}

			//PERMISSIONS
			printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
		    printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
		    printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
		    printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
		    printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
		    printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
		    printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
		    printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
		    printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");

		    //UID
		    printf(" %d ", fileStat.st_uid);

		    //SIZE
		    printf(" %*d ", 8, fileStat.st_size);

		    //CREATION TIME

		    ntime = (time_t)fileStat.st_ctime;
		    mytm = localtime(&ntime);
		    strftime(createtime, 256, "%b %d %R", mytm);

		    printf(" %s ", createtime);

		    strncpy(name, dp->name, dp->name_len);
		    name[dp->name_len] = '\0';
		    printf(" %s\n", name);
			cp += dp->rec_len;			// advance cp by rlen in bytes
			dp = (DIR *)cp;				// pull dp to the next record
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
int _link(char * fold, char * fnew){

	unsigned long ino, ino2;
	MINODE *mip;
	char oldcpy[128], newcpy[128], dpname[64];
	char cmpname[128];

	int i = 0, found = 0, new_len, need_len, ideal_len;
	INODE *inode;
	char * cp, * prev;
	DIR * dp;

	strcpy(oldcpy, fold);
	strcpy(newcpy, fnew);

	ino = getino(dev, oldcpy);
	mip = iget(dev, ino);

	if(mip->INODE.i_mode == FILE_MODE){
		ino2 = getino(dev, dirname(newcpy));

		strcpy(newcpy, fnew);
		strcpy(cmpname, basename(newcpy));
		if(ino!=0){
			// We know that the directory is there
					inode = findInode(ino2);
					found = fileExists(inode, cmpname);

					if(!found){ // Then we can add it
						 i = 0;

						 while(i<12)
						 {
						 	if(inode->i_block[i] != 0)
						 	{
						 	get_block(dev, inode->i_block[i], datablock);

							  dp = (DIR *)datablock;
							  cp = datablock; 
							  printf("end: %d\n", (int)datablock + BLOCK_SIZE);

							  while(cp < datablock + BLOCK_SIZE  && dp->rec_len != 0){
							  	prev = cp;  
							  	cp += dp->rec_len;            /* advance by rec_len */
							  	dp = (DIR *)cp;
							  }
							 
							  dp = (DIR *)prev; // dp is the last entry
							  cp = prev;
							  
							  need_len = 4*((8 + strlen(cmpname) + 3)/4);  // Get needed length for new directory

							  ideal_len = 4*((8 + dp->name_len + 3)/4);	  // Get ideal length of the last dir_entry
							  printf("need: %d\n", need_len);
							  printf("ideal: %d\n", ideal_len);
							  printf("cur: %d\n\n", dp->rec_len);
							  if(dp->rec_len - ideal_len >= need_len){

							  	new_len = dp->rec_len - ideal_len;

							  	dp->rec_len = ideal_len; // Set the last entry to the ideal length

							  	cp += dp->rec_len;	// Move to the end of the last entry

							  	dp = (DIR *)cp;
							  	printf("inumber: %d\n", ino);
							  	printf("HA");
							  	dp->inode = (unsigned long)ino;		/* Inode number */
							    strncpy(dp->name, cmpname, strlen(cmpname));    /* File name */
							    dp->name_len = strlen(cmpname);		/* Name length */
							    dp->rec_len = new_len;		/* Directory entry length */

								  }else{ // We need to allocate a new block to store this entry
								  		printf("HAHA");
								  }

								  put_block(dev, inode->i_block[i], datablock);
								 
						}
						 i++;
					}

						mip->INODE.i_links_count++;
						iput(mip);
				}
		}else{
			printf("Cannot find pathname!\n");
		}
	}

	return 0;
}
int _symlink(char *fold, char *fnew){
	unsigned long ino;

	ino = getino(dev, fold);

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

	  do_stat(&mystat, &mip->INODE, ino);

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

void do_stat(struct stat * mystat, INODE * ino, unsigned long inode){
	  mystat->st_dev = dev;
	  mystat->st_ino = inode;
	  mystat->st_mode = ino->i_mode;
	  mystat->st_nlink = ino->i_links_count;
	  mystat->st_uid = ino->i_uid;
	  mystat->st_gid = ino->i_gid;
	  mystat->st_size = ino->i_size;
	  mystat->st_blksize = BLOCK_SIZE;
	  mystat->st_blocks = ino->i_blocks;
	  mystat->st_atime = ino->i_atime;
	  mystat->st_ctime = ino->i_ctime;
	  mystat->st_mtime = ino->i_mtime;
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
      
        rec_complete(nwd);
     
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

	
	int inumber;
	int bnumber;
	int ideal_len;
	int need_len;
	int new_len;
	int i;
	DIR * dp;
	char * cp, * prev;
	char buf[BLOCK_SIZE] = {0};
	MINODE * mip;
	time_t curTime;

	INODE *itest;

	char str[64];

	pip->dirty = 1;
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
	getchar();
	printDir(2);
	/*itest = findInode(21);
	printInode(itest);
	get_block(dev, 38, datablock);
	printf("%s\n", datablock);*/
	getchar();
	mip->INODE.i_mode = DIR_MODE;		/* DIR and permissions */
	mip->INODE.i_uid  = running->uid;	/* Owner Uid */
	mip->INODE.i_gid  = running->gid;	/* Group Id */
	mip->INODE.i_size = BLOCK_SIZE;		/* Size in bytes */

	mip->INODE.i_links_count = 2;	/* Links count */

	mip->INODE.i_atime=mip->INODE.i_ctime=mip->INODE.i_mtime = time(0L); 

	mip->INODE.i_blocks = 2;     	/* Blocks count in 512-byte blocks */
	mip->dirty = 1;               /* mark dirty */
	printf("inumber: %d\n", inumber);
	printf("bnumber: %d\n", bnumber);
	for (i=0; i<15; i++)
	{
		mip->INODE.i_block[i] = 0;
	}

	mip->INODE.i_block[0] = bnumber; 

	printf("BEFORE\n");
	printInode(&mip->INODE);

	iput(mip);

	// TEST
	mip = NULL;
	mip = iget(dev, inumber);
	getchar();

	printf("AFTER\n");
	printInode(&mip->INODE);
	iput(mip);
	getchar();
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

	// TEST
	/*get_block(dev, bnumber, buf);
	dp = (DIR *)buf;
	printf("inode: %d\n", dp->inode);
	strncpy(str, dp->name, dp->name_len);
	str[dp->name_len] = '\0';

	printf("name: %s\n\n", str);
	getchar();*/

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
  get_block(dev, pip->INODE.i_block[0], datablock);

  dp = (DIR *)datablock;
  cp = datablock; 
  printf("end: %d\n", (int)datablock + BLOCK_SIZE);

  while(cp < datablock + BLOCK_SIZE  && dp->rec_len != 0){
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
  	printf("inumber: %d\n", inumber);
  	dp->inode = (unsigned long)inumber;		/* Inode number */
    strncpy(dp->name, name, strlen(name));    /* File name */
    dp->name_len = strlen(name);		/* Name length */
    dp->rec_len = new_len;		/* Directory entry length */

  }else{ // We need to allocate a new block to store this entry
  		printf("HAHA");
  }


  	put_block(dev, pip->INODE.i_block[0], datablock);

    time(&curTime);
    
    pip->INODE.i_atime = curTime;
    pip->INODE.i_links_count++;
    pip->dirty = 1;

    iput(pip);

return 1;
}
