#include "headers/type.h"

////////////////////////////////////////////////////////
//
//	Command Functions
//
////////////////////////////////////////////////////////


int _menu(char *arg, char *pathname)
{
	printf("~~~~~~~MENU~~~~~~~\n\n");
	
	printf("help\nls\ncd\nmkdir\nrmdir\npwd\ncreat\nrm\nstat\nlink\nunlink\nsymlink\ntouch\nchmod\nchown\nchgrp\nopen\nclose\nread\nwrite\npfd\nlseek\ncat\ncp\nmv\nquit");
	
	return 0;
}

int _ls(char *arg, char *pathname)
{
	MINODE *mp = proc[0].cwd;
	INODE *inode;
	struct stat fileStat;
//	int isRoot = -1;
	unsigned long ino;
	char name[BLOCK_SIZE];
	char createtime[64];
	time_t ntime;
	struct tm * mytm;
////////////////////////////////////////////////////////////////////////
	int i = 0;
////////////////////////////////////////////////////////////////////////
	
	printf("~~~~~~~~LS~~~~~~~~\n\n");
		
	
		if(strcmp(pathname, "")==0){
			mp = running->cwd;
		}else{

			ino = getino(&dev, pathname);

			if (ino == 0)
			{
				printf("Could Not Find Path: ls()\n");
				return -1;
			}

			mp = iget(mp->dev, ino, basename(pathname));
		}
		
		while(i<12){
		printf("block[%d] = %d\n", i, mp->INODE.i_block[i] );
		i++;
		}

	for(i = 0; i < 12; i++)
	{
		
		//printf("%d\n", i);
		if( mp->INODE.i_block[i] != 0)
		{
		
		get_block(dev, mp->INODE.i_block[i], datablock);
		
		
		dp = (DIR *)datablock;
		cp = datablock;
		
		
		while(cp < datablock + BLOCK_SIZE)
		{
			//printf("cp: %d-%d\n", (int)cp, i);
		
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
		    printf(" %*d ", 8, (int)fileStat.st_size);

		    //CREATION TIME

		    ntime = (time_t)fileStat.st_atime;
		    mytm = localtime(&ntime);
		    strftime(createtime, 256, "%b %d %R", mytm);

		    printf(" %s ", createtime);

		    strncpy(name, dp->name, dp->name_len);
		    name[dp->name_len] = '\0';
		    printf(" %s\n", name);
			cp += dp->rec_len;			// advance cp by rlen in bytes
			dp = (DIR *)cp;				// pull dp to the next record
			//printf("AFTER: %d\n", (int)cp);

        }
        //printf("END: %d\n", (int)datablock+BLOCK_SIZE);
    }else{
    	break;
    }

	}
	
///////////////////////////////////////////////////////////////////////////////////////////////
	
	return 0;
}

int _cd(char *arg, char *pathname)
{
	printf("~~~~~~~~CD~~~~~~~~\n\n");
	int result = -1;
  unsigned long ino, pino;
  MINODE * mip;

  // No pathname, go to root
 

  if(strcmp(pathname, "")==0){ 
    iput(running->cwd);
    running->cwd = iget(ROOT_DEV, ROOT_INODE, running->cwd->name);

    result = 0;
  }else if(strcmp(pathname, ".")==0){
    // Stay here
    running->cwd = running->cwd;
  }else if(strcmp(pathname, "..")==0){
   
    // We need to find the inode of the parent
    printDir(running->cwd->ino);
    findino(running->cwd, &ino, &pino);
   // printf("ino = %d\npino = %d\n\n", ino, pino);

    mip = iget(dev, pino, running->cwd->name);
    iput(running->cwd);
    running->cwd = mip;

  }else{

  // Get the inode of the pathname into MINODE
    ino = getino(&dev, pathname);
    printf("CD -- ino = %d @ %s\n", (int)ino, pathname);
    mip = iget(dev, ino, basename(pathname));

    // Make sure its a DIR
    if(mip->INODE.i_mode == DIR_MODE){
      //  printf("BEFORE CD:\n");
      //  printInode(&running->cwd->INODE);
        iput(running->cwd);
        running->cwd = mip;

        //printf("AFTER CD:\n");
        //printInode(&running->cwd->INODE);

        result = 0;
    }else{
        result = 1;
        printf("Given pathname is not a directory...\n");
    }

  }
    
    return result;
  }
  
int _mkdir(char *arg, char *pathname)
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
		pip = iget(dev, ino, parent);
		printInode(&pip->INODE);

		printf("i_mode: %x\nDIR_MODE: %x\n", pip->INODE.i_mode, DIR_MODE);
		if(pip->INODE.i_mode == DIR_MODE)
			printf("THIS IS A DIRECTORY!\n");
		else
			printf("THIS IS NOT A DIRECTORY\n");

		r = my_mkdir(pip, child);

	return 0;
}

int _rmdir(char *arg, char *pathname)
{
	unsigned long ino, pino;
	MINODE * pip, * mip;
	int i;

	printf("~~~~~~RMDIR~~~~~~~\n\n");

	printf("pathname: %s\n", pathname);
	ino = getino(&dev, pathname);
	if(ino==0){
		printf("Cannot find inode..\n");
		return -1;
	}
	mip = iget(dev, ino, pathname);

	if(mip->INODE.i_mode == DIR_MODE && running->status != BUSY && isDirEmpty(mip)){

		printf("Success\n");

		 for (i=0; i<12; i++){

         if (mip->INODE.i_block[i]!=0){
	         bdealloc(mip->dev, mip->INODE.i_block[i]);
	    	 
		     }else{
		     	break;
		     }
		 } 

	     idealloc(mip->dev, mip->ino);
	     iput(mip);

	     findino(mip, &ino, &pino);


	     pip = iget(dev, pino, pathname);

	     rm_child(pip, basename(pathname));

	     pip->INODE.i_atime = pip->INODE.i_mtime = time(0L);

	     pip->dirty = 1;
	     iput(pip);
	     return 1;

	}
	return 0;
}

int rm_child(MINODE * pip, char *myname)
{
	int i=0;
	DIR * dp, *next, *last;
	char * cp, * np;
	int del_len;
	char mname[256];

	if(strncmp(myname, ".", 1)==0 || strncmp(myname, "..", 2)==0)
	{
		printf("Cannot remove . or ..\n");
		return 0;
	}
				
				printf("myname: %s\n", myname);	
	while(i<12)
	{

		if(pip->INODE.i_block[i] != 0)
		{
			
			get_block(dev, pip->INODE.i_block[i], datablock);
			dp = (DIR *)datablock;
			cp = datablock;

			while(cp < datablock + BLOCK_SIZE)
			{
				strncpy(mname, dp->name, dp->name_len);
				mname[dp->name_len] = '\0';

				printf("dp->ino: %d\n", (int)dp->inode);
				printf("dp->name: %s\n", mname);
				printf("dp->len: %d\n", dp->name_len );
				printf("dp->rec: %d\n\n", dp->rec_len);
				if(strncmp(dp->name, myname, dp->name_len)==0)
				{
						del_len = dp->rec_len;
						if(del_len == BLOCK_SIZE)
						{
							bdealloc(dev, pip->INODE.i_block[i]);
							pip->INODE.i_size -= BLOCK_SIZE;
							pip->INODE.i_block[i] = 0;
							printf("DEALLOCATED BLOCK: %d", (int)pip->INODE.i_block[i]);
							return 0;
						}
						break;
				}
				last = (DIR *)cp;

				cp += dp->rec_len;
				dp = (DIR *)cp;
				np = cp + dp->rec_len;
				next = (DIR *)np;
			}

			// Now we found the file at dp.. the file after is next
			
			while(np < datablock + BLOCK_SIZE)
			{
				memcpy(dp, np, next->rec_len);
				last = (DIR *)cp;

				cp += dp->rec_len;

				dp = (DIR *)cp;

				np = cp + del_len;
				next = (DIR *)np;
			}

			last->rec_len += del_len;
			put_block(dev, pip->INODE.i_block[i], datablock);
		}
		i++;
	}
	return 0;
}

int isDirEmpty(MINODE * pip)
{
	int empty = 1;
	DIR * dp;
	char * cp;

	get_block(dev, pip->INODE.i_block[0], datablock);

	dp = (DIR *)datablock;
	cp = datablock;

	while(cp < datablock + BLOCK_SIZE)
	{
		if((strncmp(dp->name, ".", 1)!=0) && (strncmp(dp->name, "..", 2)!=0))
		{
			printf("DIR is not empty\n");
			empty = 0;
			break;
		}
		cp += dp->rec_len;         // advance cp by rlen in bytes
		dp = (DIR *)cp;       // pull dp to the next record
	}

	return empty;
}

int _pwd(char *arg, char *pathname)
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

int _creat0(char *arg, char *pathname)
{
	printf("~~~~~~CREAT~~~~~~~\n\n");
	char * parent, * child;
    char fullpath[128], fullpath2[128];
    int r;
    unsigned long ino;
    MINODE * pip;
  
    // Create a copy of the pathname so we don't alter it directly
    strcpy(fullpath, pathname);
    strcpy(fullpath2, pathname);
  
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

	

		printf("fullPath: %s\n", fullpath);
		parent = dirname(fullpath); 
		child  = basename(fullpath2);

		printf("parent: %s\n", parent);
		printf("child: %s\n", child);

		ino = getino(&dev, parent);

		// Get the MINODE of the parent
		pip = iget(dev, ino, parent);
		printInode(&pip->INODE);

		printf("i_mode: %x\nDIR_MODE: %x\n", pip->INODE.i_mode, DIR_MODE);
		if(pip->INODE.i_mode == DIR_MODE){
			printf("THIS IS A DIRECTORY!\n");
		}else{
			printf("THIS IS NOT A DIRECTORY\n");
		}

		r = my_creat(pip, child);
	
	
	return 0;
}

int _rm(char *arg, char *pathname)
{
	
	unsigned long ino, pino;
	MINODE * pip, * mip;
	int i;
	char buffer[256];

	printf("~~~~~~~~RM~~~~~~~~\n\n");

	ino = getino(&dev, pathname);
	mip = iget(dev, ino, pathname);

	if(mip->INODE.i_mode == FILE_MODE && running->status != BUSY){

		

		 for (i=0; i<12; i++){

         if (mip->INODE.i_block[i]!=0){
	         bdealloc(mip->dev, mip->INODE.i_block[i]);
	    	 
		     }else{
		     	break;
		     }
		 } 

	     idealloc(mip->dev, mip->ino);
	     iput(mip);

	     strcpy(buffer, pathname);
	     pino = getino(&dev, dirname(buffer));
	    
	     pip = iget(dev, pino, pathname);
	     
	     rm_child(pip, basename(pathname));

	     pip->INODE.i_atime = pip->INODE.i_mtime = time(0L);

	     pip->dirty = 1;
	     iput(pip);
	     return 1;

	}
	return 0;
}

int _touch(char *arg, char *pathname)
{
	MINODE *mip, *pip;
	int ino = 0;
	char buf[256];
	char * child;

	printf("~~~~~~TOUCH~~~~~~~\n\n");

	// Get the inode of the pathname into MINODE
	if (strcmp(pathname, "")==0)
	{
		printf("Not given anything to touch.\nExiting\n");
		return -1;
	}
	else
	{
		ino = getino(&dev, pathname);

		if (ino == 0)
		{
			strcpy(buf, pathname);
			ino = getino(&dev, dirname(buf));
			strcpy(buf, pathname);
			child = basename(buf);
			pip = iget(dev, ino, child);

			my_creat(pip, child);

			//printf("Failed retreiving ino: touch()\n");
			return 1;
		}
		printf("touch -- ino = %d\n", (int)ino);
		mip = iget(dev, ino, basename(pathname));

printf("\nTest (Before):\nmip->INODE.i_atime = %u\nmip->INODE.i_mtime = %u\n\n", mip->INODE.i_atime, mip->INODE.i_mtime);

		mip->INODE.i_atime = time(0L);
		mip->INODE.i_mtime = time(0L);
		mip->dirty = 1;

printf("\nTest (After):\nmip->INODE.i_atime = %u\nmip->INODE.i_mtime = %u\n\n", mip->INODE.i_atime, mip->INODE.i_mtime);

		iput(mip);
	}

	return 0;
}

int _link(char * arg, char * pathname)
{
	unsigned long ino, ino2;
	MINODE *mip;
	char oldcpy[128], newcpy[128], dpname[64];
	char cmpname[128];

	int i = 0, found = 0, new_len, need_len, ideal_len;
	INODE *inode;
	char * cp, * prev;
	DIR * dp;

	printf("~~~~~~LINK~~~~~~~\n\n");
	strcpy(oldcpy, pathname);
	strcpy(newcpy, arg);

	ino = getino(&dev, oldcpy);
	mip = iget(dev, ino, oldcpy);

	if(mip->INODE.i_mode == FILE_MODE)
	{
		ino2 = getino(&dev, dirname(newcpy));

		strcpy(newcpy, arg);
		strcpy(cmpname, basename(newcpy));
		getchar();
		if(ino!=0)
		{
			// We know that the directory is there
			inode = findInode(ino2);
			found = fileExists(inode, cmpname);


			if(!found)
			{ // Then we can add it
				i = 0;

				while(i<12)
				{
					if(inode->i_block[i] != 0)
					{
						get_block(dev, inode->i_block[i], datablock);

						dp = (DIR *)datablock;
						cp = datablock; 
						printf("end: %d\n", (int)datablock + BLOCK_SIZE);

						while(cp < datablock + BLOCK_SIZE  && dp->rec_len != 0)
						{
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
						if(dp->rec_len - ideal_len >= need_len)
						{

							new_len = dp->rec_len - ideal_len;

							dp->rec_len = ideal_len; // Set the last entry to the ideal length

							cp += dp->rec_len;	// Move to the end of the last entry

							dp = (DIR *)cp;

							dp->inode = (unsigned long)ino;		/* Inode number */
							strncpy(dp->name, cmpname, strlen(cmpname));    /* File name */
							dp->name_len = strlen(cmpname);		/* Name length */
							dp->rec_len = new_len;		/* Directory entry length */

						}
						else
						{ // We need to allocate a new block to store this entry
							printf("HAHA");
						}

						put_block(dev, inode->i_block[i], datablock);

					}
					i++;
				}

				mip->INODE.i_links_count++;
				iput(mip);
			}
		}
		else
		{
			printf("Cannot find pathname!\n");
		}
	}

	return 0;
}

int _unlink(char *arg, char *pathname)
{
	unsigned long ino;
	MINODE * mip, *pip;
	int i = 0;
	char path[64];
	char * child;
	printf("---------UNLINK----------\n");
	strcpy(path, pathname);

	ino = getino(&dev, pathname);
	mip = iget(dev, ino, basename(path));
	strcpy(path, pathname);
	printf("path: %s\n", path);
	if(mip->INODE.i_mode != DIR_MODE)
	{
		mip->INODE.i_links_count--;

		if(mip->INODE.i_links_count == 0)
		{
			// Deallocate the blocks
			while(i<12)
			{
				if(mip->INODE.i_block[i] != 0)
				{
					bdealloc(dev, mip->INODE.i_block[i]);
				}
				else
				{
					break;
				}
				i++;
			}

			// Deallocate the inode
			idealloc(dev, ino);
		}

		ino = getino(&dev, dirname(path));
		strcpy(path, pathname);
		pip = iget(dev, ino, dirname(path));
		strcpy(path, pathname);

		printf("%s-\n", path);
		child = basename(path);
		printf("child: %s\n", child );
		rm_child(pip, child);
	}
	return 0;
}

int _symlink(char *arg, char *pathname)
{
	unsigned long ino;
	char old[128], new[128], new2[128];
	char * child, * parent;

	int i = 0, found = 0, new_len, need_len, ideal_len;
	INODE *inode;
	char * cp, * prev;
	DIR * dp;
	MINODE * mip, *created;
	MINODE * pip;

	printf("~~~~~~SYMLINK~~~~~~~\n\n");
	strcpy(old, pathname);
	strcpy(new, arg);
	strcpy(new2, arg);

	ino = getino(&dev, old);
	mip = iget(dev, ino, old);

	if(mip != NULL)
	{
		// We know the old file exists
		parent = dirname(new);
		
		child = basename(new2);
		printf("pathname: %s\n", pathname);
		printf("arg: %s\n%s", arg, new);
		printf("parent: %s\n", parent);
		printf("child: %s\n", child);

		ino = getino(&dev, parent);
		pip = iget(dev, ino, parent);

		ino = my_creat(pip, child); // ino is now the ino of the new file

		created = iget(dev, ino, child);

		created->INODE.i_mode = SYM_MODE;

		memcpy(&created->INODE.i_block[0], old, strlen(old)+1);

		iput(created);
		iput(pip);
		iput(mip);
	}

	return 0;
}

int _chmod(char *arg, char *pathname)
{
	MINODE *mip;
	int add, user, group, others, nread, nwrite, nexecute, ino, len, i, mode, mode2, result1, result2, result3;
	int ur, uw, ux, gr, gw, gx, or, ow, ox;
	add = user = group = others = nread = nwrite = nexecute = -1;
	ur = uw = ux = gr = gw = gx = or = ow = ox = 0;
	ino = len = i = mode = mode2 = result1 = result2 = result3 = 0;
//	char *tmp;
	
	printf("~~~~~~CHMOD~~~~~~~\n\n");
	
//	tmp = arg;
//	arg = pathname;
//	pathname = tmp;
	
	
	if (strcmp(pathname, "")==0)
	{
		printf("Not given a file to change the mode of.\nExiting\n");
		return -1;
	}
	
	ino = getino(&dev, pathname);
	if (ino == 0)
	{
		printf("Failed retreiving ino: chmod()\n");
		return -1;
	}
	mip = iget(dev, ino, basename(pathname));
	
	// convert current file permissions to octal
	mode = mip->INODE.i_mode;
printf("\nTest (Before):\nmip->INODE.i_mode = %d\n\n", mip->INODE.i_mode);
	
	result1 = 0;
	i = 1;
	while (mode)
	{
		result1 += (mode % 8) * i;
		mode /= 8;
		i *= 10;
	}
	
	// find current file permissions
	mode2 = result1;
	mode2 %= 10;
	if ((mode2 % 8) > 3)
		or = 1;
	else if ((mode2 % 8) < 4)
		or = 0;
	if ( ((mode2 % 8) > 5) || ((mode2 % 8) == 3) || ((mode2 % 8) == 2) )
		ow = 1;
	else if ( ((mode2 % 8) < 2) || ((mode2 % 8) == 4) || ((mode2 % 8) == 5) )
		ow = 0;
	if ((mode2 % 2) == 1)
		ox = 1;
	else if ((mode2 % 2) == 0)
		ox = 0;
	mode2 = result1/10;
	mode2 %= 10;
	if ((mode2 % 8) > 3)
		gr = 1;
	else if ((mode2 % 8) < 4)
		gr = 0;
	if ( ((mode2 % 8) > 5) || ((mode2 % 8) == 3) || ((mode2 % 8) == 2) )
		gw = 1;
	else if ( ((mode2 % 8) < 2) || ((mode2 % 8) == 4) || ((mode2 % 8) == 5) )
		gw = 0;
	if ((mode2 % 2) == 1)
		gx = 1;
	else if ((mode2 % 2) == 0)
		gx = 0;
	mode2 = result1/100;
	mode2 %= 10;
	if ((mode2 % 8) > 3)
		ur = 1;
	else if ((mode2 % 8) < 4)
		ur = 0;
	if ( ((mode2 % 8) > 5) || ((mode2 % 8) == 3) || ((mode2 % 8) == 2) )
		uw = 1;
	else if ( ((mode2 % 8) < 2) || ((mode2 % 8) == 4) || ((mode2 % 8) == 5) )
		uw = 0;
	if ((mode2 % 2) == 1)
		ux = 1;
	else if ((mode2 % 2) == 0)
		ux = 0;
printf("before: %d%d%d%d%d%d%d%d%d\n", ur, uw, ux, gr, gw, gx, or, ow, ox);
	// process argument to determine what is in it
	len = strlen(arg);
	for (i = 0; i < len; i++)
	{
		if (strncmp(&arg[i], "+", 1)==0)
			add = 1;
		else if (strncmp(&arg[i], "-", 1)==0)
			add = 0;
		if (strncmp(&arg[i], "a", 1)==0)
			user = group = others = 1;
		if (strncmp(&arg[i], "u", 1)==0)
			user = 1;
		if (strncmp(&arg[i], "g", 1)==0)
			group = 1;
		if (strncmp(&arg[i], "o", 1)==0)
			others = 1;
		if (strncmp(&arg[i], "r", 1)==0)
			nread = 1;
		if (strncmp(&arg[i], "w", 1)==0)
			nwrite = 1;
		if (strncmp(&arg[i], "x", 1)==0)
			nexecute = 1;
	}
	
	// manipulate permissions
	if (add == 1)			// adding permission(s)
	{
		//user
		if (user == 1)
		{
			if (nread == 1)
				ur = 1;
			if (nwrite == 1)
				uw = 1;
			if (nexecute == 1)
				ux = 1;
		}
		
		//group
		if (group == 1)
		{
			if (nread == 1)
				gr = 1;
			if (nwrite == 1)
				gw = 1;
			if (nexecute == 1)
				gx = 1;
		}
		
		//others
		if (others == 1)
		{
			if (nread == 1)
				or = 1;
			if (nwrite == 1)
				ow = 1;
			if (nexecute == 1)
				ox = 1;
		}
	}
	else if (add == 0)		// removing permission(s)
	{
		//user
		if (user == 1)
		{
			if (nread == 1)
				ur = 0;
			if (nwrite == 1)
				uw = 0;
			if (nexecute == 1)
				ux = 0;
		}
		
		//group
		if (group == 1)
		{
			if (nread == 1)
				gr = 0;
			if (nwrite == 1)
				gw = 0;
			if (nexecute == 1)
				gx = 0;
		}
		
		//others
		if (others == 1)
		{
			if (nread == 1)
				or = 0;
			if (nwrite == 1)
				ow = 0;
			if (nexecute == 1)
				ox = 0;
		}
	}
	else if (add == -1)	// numerical permissions
	{
		if ((strlen(arg) != 3) || (strncmp(&arg[0], "8", 1)==0)
		|| (strncmp(&arg[0], "9", 1)==0) || (strncmp(&arg[1], "8", 1)==0)
		|| (strncmp(&arg[1], "9", 1)==0) || (strncmp(&arg[2], "8", 1)==0)
		|| (strncmp(&arg[2], "9", 1)==0))
		{
			printf("Incorrect format for argument (%s): chmod()\n", arg);
			return -1;
		}
		result1 /= 1000;
		result1 *= 1000;
		result1 += atoi(arg);
	}
	if ((add == 0) || (add == 1))
	{
		//alter result1 to reflect the changes made to permissions
		result1 /= 1000;
		result1 *= 10;
		// add user permission octal
		if (ur == 1 && uw == 1 && ux == 1)
			result1 += 7;
		else if (ur == 1 && uw == 1 && ux == 0)
			result1 += 6;
		else if (ur == 1 && uw == 0 && ux == 1)
			result1 += 5;
		else if (ur == 1 && uw == 0 && ux == 0)
			result1 += 4;
		else if (ur == 0 && uw == 1 && ux == 1)
			result1 += 3;
		else if (ur == 0 && uw == 1 && ux == 0)
			result1 += 2;
		else if (ur == 0 && uw == 0 && ux == 1)
			result1 += 1;
		else if (ur == 0 && uw == 0 && ux == 0)
			result1 += 0;
		result1 *= 10;
		// add group permission octal
		if (gr == 1 && gw == 1 && gx == 1)
			result1 += 7;
		else if (gr == 1 && gw == 1 && gx == 0)
			result1 += 6;
		else if (gr == 1 && gw == 0 && gx == 1)
			result1 += 5;
		else if (gr == 1 && gw == 0 && gx == 0)
			result1 += 4;
		else if (gr == 0 && gw == 1 && gx == 1)
			result1 += 3;
		else if (gr == 0 && gw == 1 && gx == 0)
			result1 += 2;
		else if (gr == 0 && gw == 0 && gx == 1)
			result1 += 1;
		else if (gr == 0 && gw == 0 && gx == 0)
			result1 += 0;
		result1 *= 10;
		// add others permission octal
		if (or == 1 && ow == 1 && ox == 1)
			result1 += 7;
		else if (or == 1 && ow == 1 && ox == 0)
			result1 += 6;
		else if (or == 1 && ow == 0 && ox == 1)
			result1 += 5;
		else if (or == 1 && ow == 0 && ox == 0)
			result1 += 4;
		else if (or == 0 && ow == 1 && ox == 1)
			result1 += 3;
		else if (or == 0 && ow == 1 && ox == 0)
			result1 += 2;
		else if (or == 0 && ow == 0 && ox == 1)
			result1 += 1;
		else if (or == 0 && ow == 0 && ox == 0)
			result1 += 0;
	}

	//convert octal back to decimal and store it
printf("intermediate result = %d\n", result1);
	result2 = 0;
	i = 0;
	result3 = 1;
	mode = result1;
	while (mode)
	{
		if (i != 0)
			result3 *= 8;
		result2 += (mode % 10) * result3;
		mode /= 10;
		i++;
	}
	
	mip->INODE.i_mode = result2;
	mip->dirty = 1;
printf("after:  %d%d%d%d%d%d%d%d%d\n", ur, uw, ux, gr, gw, gx, or, ow, ox);
printf("\nTest (After):\nmip->INODE.i_mode = %d\n\n", mip->INODE.i_mode);
	
	iput(mip);
	
	return 0;
}

int _chown(char *arg, char *pathname)
{
	MINODE *mip;
	int ino = 0;
	
	printf("~~~~~~CHOWN~~~~~~~\n\n");
	
	if (strcmp(pathname, "")==0)
	{
		printf("Not given a file to change the owner of.\nExiting\n");
		return -1;
	}
	else
	{
		ino = getino(&dev, pathname);
		if (ino == 0)
		{
			printf("Failed retreiving ino: chown()\n");
			return -1;
		}
		printf("chown -- ino = %d\n", (int)ino);
		mip = iget(dev, ino, basename(pathname));
		
printf("\nTest (Before):\nmip->INODE.i_uid = %u\n\n", mip->INODE.i_uid);
		
		mip->INODE.i_uid = atoi(arg);
		mip->dirty = 1;
		
printf("\nTest (After):\nmip->INODE.i_uid = %u\n\n", mip->INODE.i_uid);
		
		iput(mip);
	}
	
	return 0;
}

int _chgrp(char *arg, char *pathname)
{
	MINODE *mip;
	int ino = 0;
	
	printf("~~~~~~CHGRP~~~~~~~\n\n");
	
	if (strcmp(pathname, "")==0)
	{
		printf("Not given a file to change the group of.\nExiting\n");
		return -1;
	}
	else
	{
		ino = getino(&dev, pathname);
		if (ino == 0)
		{
			printf("Failed retreiving ino: chgrp()\n");
			return -1;
		}
		printf("chgrp -- ino = %d\n", (int)ino);
		mip = iget(dev, ino, basename(pathname));
		
printf("\nTest (Before):\nmip->INODE.i_gid = %u\n\n", mip->INODE.i_gid);
		
		mip->INODE.i_gid = atoi(arg);
		mip->dirty = 1;
		
printf("\nTest (After):\nmip->INODE.i_gid = %u\n\n", mip->INODE.i_gid);
		
		iput(mip);
	}
	
	return 0;
}

int _open (char *arg, char *pathname)
{
	printf("~~~~~~~OPEN~~~~~~~\n\n");
	
//	1. ask for a pathname and mode to open:
//         You may use mode = 0|1|2|3 for R|W|RW|APPEND
//
//  2. get pathname's inumber:
//         ino = getino(&dev, pathname);
//
//  3. get its Minode pointer
//         mip = iget(dev,ino);  
//
//  4. check mip->INODE.i_mode to verify it's a REGULAR file and permission OK.
//     (Optional : do NOT check FILE type so that we can open DIRs for RW)
//     
//     Check whether the file is ALREADY opened with INCOMPATIBLE type:
//           If it's already opened for W, RW, APPEND : reject.
//           (that is, only multiple R are OK)
//
//  5. allocate an OpenFileTable (OFT) entry and fill in values:
//         oftp = falloc();       // get a FREE OFT
//         oftp->mode = mode;     // open mode 
//         oftp->refCount = 1;
//         oftp->inodeptr = mip;  // point at the file's minode[]
//
//  6. Depending on the open mode 0|1|2|3, set the OFT's offset accordingly:
//
//      switch(mode){
//         case 0 : oftp->offset = 0; 
//                  break;
//         case 1 : truncate(mip);        // W : truncate file to 0 size
//                  oftp->offset = 0;
//                  break;
//         case 2 : oftp->offset = 0;    // RW does NOT truncate file
//                  break;
//         case 3 : oftp->offset =  mip->INODE.i_size;  // APPEND mode
//                  break;
//         default: printf("invalid mode\n");
//                  return(-1);
//      }
//
//   7. find the SMALLEST i in running PROC's fd[ ] such that fd[i] is NULL
//      Let running->fd[i] point at the OFT entry
//
//   8. update INODE's time field. 
//      for W|RW|APPEND mode : mark Minode[] dirty
//
//   9. return i as the file descriptor
	
	return 0;
}

int _close (char *arg, char *pathname)
{
	printf("~~~~~~CLOSE~~~~~~~\n\n");
	
	
	
	return 0;
}

int _read (char *arg, char *pathname)
{
	printf("~~~~~~~READ~~~~~~~\n\n");
	
	
	
	return 0;
}

int _write (char *arg, char *pathname)
{
	printf("~~~~~~WRITE~~~~~~~\n\n");
	
	
	
	return 0;
}

int _pfd (char *arg, char *pathname)
{
	printf("~~~~~~~~PFD~~~~~~~\n\n");
	
	
	
	return 0;
}

int _lseek (char *arg, char *pathname)
{
	printf("~~~~~~LSEEK~~~~~~~\n\n");
	
	
	
	return 0;
}

int _cat (char *arg, char *pathname)
{
	printf("~~~~~~~~CAT~~~~~~~\n\n");
	
	
	
	return 0;
}

int _cp (char *arg, char *pathname)
{
	printf("~~~~~~~~CP~~~~~~~~\n\n");
	
	
	
	return 0;
}

int _mv (char *arg, char *pathname)
{
	printf("~~~~~~~~MV~~~~~~~~\n\n");
	
	
	
	return 0;
}

int _stat(char *arg, char *pathname)
{
	struct stat mystat;
	unsigned long ino;
	MINODE * mip;

	printf("before:%s\n", pathname);

	ino = getino(&dev, pathname);
	printf("middle:%s\n", pathname);
	mip = iget(dev, ino, basename(pathname));

	do_stat(&mystat, &mip->INODE, ino);

	printf("after:%s\n", pathname);
	printf("---- stat - %s ----\n\n", basename(pathname));
	printf("Device: %d\n", (int)mystat.st_dev);
	printf("Inode: %d\n", (int)mystat.st_ino);
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

	printf("Links: %d\n", (int)mystat.st_nlink);
	printf("UID: %d\n", (int)mystat.st_uid);
	printf("GID: %d\n", (int)mystat.st_gid);
	printf("Size: %d\n", (int)mystat.st_size);
	printf("Blocksize: %d\n", (int)mystat.st_blksize);
	printf("# of Blocks: %d\n", (int)mystat.st_blocks);
	printf("aTime: %s", ctime(&mystat.st_atime));
	printf("cTime: %s", ctime(&mystat.st_ctime));
	printf("mTime: %s", ctime(&mystat.st_mtime));
	printf("\n");

	iput(mip);
	
	return 0;
}

void do_stat(struct stat * mystat, INODE * ino, unsigned long inode)
{
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

int __exit(char *arg, char *pathname)
{
	printf("~~~~~QUITTING~~~~~\n\n");
	quit();
	exit(0);
	return 0;
}

int rec_pwd(MINODE *wd)
{
	unsigned long pino = 0, ino = wd->ino;
	MINODE * nwd;
	char buf[BLOCK_SIZE];
	char name[256];
	int c = 0;

	if(wd->ino == ROOT_INODE)
	{
		//printf("");
	}
	else
	{
		get_block(dev, wd->INODE.i_block[0], buf);
		// Iterate through dir entries to find ..
		dp = (DIR *)buf;
		cp = buf; 

		while(cp < buf + BLOCK_SIZE)
		{
			if(c==1)
			{
				pino = dp->inode;
				printf("FOUND PARENT - %d\n", (int)pino);
			}
			cp += dp->rec_len;            /* advance by rec_len */
			dp = (DIR *)cp;
			c++;
		}

		if(pino == 0)
		{
			printf("Error finding '..'\n");
			return -1;
		}
		else
		{
			nwd = iget(dev, pino, wd->name);

			rec_pwd(nwd);

			findmyname(nwd, ino, &name[0]);

			printf("/%s", name);
		}
	}

	return 0;
}

int rec_complete(MINODE *wd)
{
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
     
        nwd = iget(dev, pino, wd->name);
      
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
	int done = 0;
	int i, bIndex;
	DIR * dp;
	char * cp, * prev;
	char buf[BLOCK_SIZE] = {0};
	MINODE * mip;
	time_t curTime;
	int needNew = 0;
	unsigned long ablock;

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
	mip = iget(dev,inumber, name);

	/*itest = findInode(21);
	printInode(itest);
	get_block(dev, 38, datablock);
	printf("%s\n", datablock);*/

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
i=0;
while(i<12)
{
	if(pip->INODE.i_block[i+1] == 0)
	{
	  get_block(dev, pip->INODE.i_block[i], datablock);

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
	  	done = 1;
	  }else{ // We need to allocate a new block to store this entry
	  		bIndex = i+1;
	  		
	  		ablock = balloc(dev);

	  		pip->INODE.i_block[bIndex] = ablock;

	  		get_block(dev, ablock, block);

	  		dp = (DIR *)block;
	  		cp = block;

	  		dp->inode = (unsigned long)inumber;
	  		strncpy(dp->name, name, strlen(name));    /* File name */
	   		dp->name_len = strlen(name);		/* Name length */
	   		dp->rec_len = BLOCK_SIZE;		/* Directory entry length */

	  		put_block(dev, ablock, block);

	  		pip->INODE.i_size += BLOCK_SIZE;

	   		done = 1;
	  }


	  	put_block(dev, pip->INODE.i_block[i], datablock);
	  	if(done){break;}

	  }
	  i++;
}
    time(&curTime);
    
    pip->INODE.i_atime = curTime;
    pip->INODE.i_links_count++;
    pip->dirty = 1;

    iput(pip);

return 1;
}

int my_creat(MINODE *pip, char *name)
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
	int i, done = 0;
	int bIndex;
	DIR * dp;
	char * cp, * prev;
	char buf[BLOCK_SIZE] = {0};
	MINODE * mip;
	time_t curTime;
	int ablock;

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
	mip = iget(dev,inumber, name);

	/*itest = findInode(21);
	printInode(itest);
	get_block(dev, 38, datablock);
	printf("%s\n", datablock);*/

	mip->INODE.i_mode = FILE_MODE;		/* REG and permissions */
	mip->INODE.i_uid  = running->uid;	/* Owner Uid */
	mip->INODE.i_gid  = running->gid;	/* Group Id */
	mip->INODE.i_size = 0;		/* Size in bytes */

	mip->INODE.i_links_count = 1;	/* Links count */

	mip->INODE.i_atime=mip->INODE.i_ctime=mip->INODE.i_mtime = time(0L); 

	mip->INODE.i_blocks = 2;     	/* Blocks count in 512-byte blocks */
	mip->dirty = 1;               /* mark dirty */

	for (i=0; i<15; i++)
	{
		mip->INODE.i_block[i] = 0;
	}

	mip->INODE.i_block[0] = bnumber; 

	iput(mip);
	
  /*


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
i=0;
while(i<12)
{
	if(pip->INODE.i_block[i+1] == 0)
	{
  get_block(dev, pip->INODE.i_block[i], datablock);

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
  	done = 1;
  }else{ // We need to allocate a new block to store this entry
  		bIndex = i+1;
	  		
	  		ablock = balloc(dev);

	  		pip->INODE.i_block[bIndex] = ablock;

	  		get_block(dev, ablock, block);

	  		dp = (DIR *)block;
	  		cp = block;

	  		dp->inode = (unsigned long)inumber;
	  		strncpy(dp->name, name, strlen(name));    /* File name */
	   		dp->name_len = strlen(name);		/* Name length */
	   		dp->rec_len = BLOCK_SIZE;		/* Directory entry length */

	  		put_block(dev, ablock, block);

	  		pip->INODE.i_size += BLOCK_SIZE;

	   		done = 1;
  }


  	put_block(dev, pip->INODE.i_block[i], datablock);
  	if(done){break;}
	}
i++;
}
    time(&curTime);
    
    pip->INODE.i_atime = curTime;
    pip->dirty = 1;

    iput(pip);

return inumber;
}
