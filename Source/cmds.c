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

int _ls(char *pathname)
{
	printf("~~~~~~~~LS~~~~~~~~\n\n");
	
	
	
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

int __exit(char *pathname)
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
		        mip->dirty = 1;  /* mark dirty 

		5. call  iput(mip);  
		   which should write the new INODE out to disk. 
   */

  // C CODE:
  //**********************************************************************

	
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
  pip->dirty = 1;

  inumber = ialloc(dev);
  bnumber = balloc(dev);


/*typedef struct Minode{		
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



  mip->INODE.i_mode = 0x41ED;		/* DIR and permissions */
  mip->INODE.i_uid  = running->uid;	/* Owner Uid */
  mip->INODE.i_gid =  running->gid;	/* Group Id */
  mip->INODE.i_size = 1024 ;		/* Size in bytes */

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
//****************************************************************  */
  dp = (DIR *)buf;

  dp->inode = inumber;		/* Inode number */
  strncpy(dp->name, ".", 1);    /* File name */
  dp->name_len = 1;		/* Name length */
  dp->rec_len = 12;		/* Directory entry length */

  cp = buf; 
  cp += dp->rec_len;            /* advance by rec_len */
  dp = (DIR *)cp;

  dp->inode = pip->ino;      /* Inode number of parent DIR */
  dp->name_len = 2;		/* Name length */
  strncpy(dp->name, "..", 2);   /* File name */
  dp->rec_len = BLOCK_SIZE - 12;/* last DIR entry length to end of block */

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