#include "headers/type.h"

////////////////////////////////////////////////////////
//
//	Utility functions
//
////////////////////////////////////////////////////////
int get_block(int dev, int blockNumber, char * buf)
{
	lseek(fd,(long)BLOCK_SIZE*blockNumber, 0);
	read(fd, buf, BLOCK_SIZE);
	
	return 0;
}

void put_block(int dev, int blockNumber, char *buf)
{
	lseek(fd, (long)BLOCK_SIZE*blockNumber, 0);
	write(fd, buf, BLOCK_SIZE);
	
	return;
}

void token_path(char *pathname)
{
//  This function breaks up a pathname, such as /a/b/c/d, into components
//  a   b   c   d  and  determines the number of components n.
//  The components will be used to search for the inode of pathname.
	char pathString[128] = "";
    char *token;
    strcpy(pathString, pathname);
    n = 0;
    printf("\n");
    token = strtok(pathString, "/");
    
	while (token != NULL)
	{
		strcpy(name[n], token);
//		printf("token%d = %s\n", n, token);
		n++;
		token = strtok(NULL, "/");
	}
    printf("n = %d\n", n);
	
	return;
}

unsigned long getino(int *dev, char *pathname)
{
	// Q: I DO NOT KNOW WHAT int * dev is ???
	// A: it has to do with what device we are working with
	//    (for Tier III we will have to specify which device)

	char *tok;
	unsigned long inumber = 0;
	INODE *next = &root->INODE;

	char copy[256];

	strcpy(copy, pathname);

	tok = strtok(copy, "/");
	if(tok==NULL)
	{
		printf("first tok is NULL\n");
		inumber = minode[0].ino; // If tok is NULL then the pathname is just '/', therefore the inumber is the root ino or 2
	}
	else
	{
		while(tok != NULL)
		{
			printf("tok: %s\n", tok);
			inumber = isearch(next, tok);

			if(inumber == 0)
			{
				printf("Path cannot be found\n");
				return inumber;
			}

			next = findInode(inumber);
			printf("Path found!\n");
			tok = strtok(NULL, "/");
		}
	}

	printf("Search done inum=%lu\n", inumber);

	return inumber;
}

unsigned long search(MINODE *mip, char *name)
{
	unsigned long inumber = 0;
//   This function searches the data blocks of a DIR inode (inside an Minode[])
//   for name. You may assume DIRECT data blocks only.
printf("search() name = %s\n", name);
	inumber = isearch(&mip->INODE, name);

	return inumber;
}

//  Once you have the (dev, ino) of an inode, you may load the inode into a slot
//  in the Minode[] array. To ensure uniqueness, you must search the Minode[] 
//  array to see whether the needed INODE is already loaded.
//
//  If you find the needed INODE already in a Minode[] slot, just inc its 
//  refCount by 1 and return the Minode[] pointer.
//
//  If you do not find it in memory, you must allocate a FREE Minode[i], load
//  the INODE from disk into that Minode[i].INODE, initialize the Minode[]'s
//  other fields and return its address as a MINODE pointer,
MINODE *iget(int dev, unsigned long ino)
{
	MINODE *tmpMINode;
	INODE *tmpInode = findInode(ino);
		
//printInode(tmpInode);

	int i;
	int freeINode = -1; // location of first free MINODE
	
	for (i = 0; i < NMINODES; i++)
	{
		// We have 2 cases, if the refCount is 0, we found an open spot; else find itself
		if (minode[i].refCount == 0)
		{
			if (freeINode == -1) 
				freeINode = i;
		}
		else
		{
			if (minode[i].ino == ino)
			{
				printf("Found used MINODE[%d]\n", i);
				minode[i].refCount++;
				return &minode[i];
			}
		}
	}
	if (freeINode>=0)
	{
		printf("Found free MINODE[%d]\n", freeINode);
	}
//	printf("i_size = %d, ino = %lu, dev = %u, freeINode = %d\n", tmpInode->i_size, ino, dev, freeINode);
	tmpMINode = new_MINODE(tmpInode, ino, freeINode, dev);
	
	return tmpMINode;
}

void iput(MINODE *mip)
{
	int ino;
	INODE *inode = NULL;
	char datablock[BLOCK_SIZE];

	mip->refCount--;
	printf("\tMinode name: %s\n", mip->name);
	printf("\trefCount: %d\n", mip->refCount);
	printf("\tdirty: %d\n", mip->dirty);
	printf("\tino: %lu\n", mip->ino);
	if (mip->refCount > 0)
		return;
	else if (mip->dirty == 0)
		return;
	// refCount is 0 and dirty is 1 ==> write the block to disk
	
	// TODO: write the block to disk
	if (mip->dev != root->dev)	// currently loaded device is different
	{							// than the one we want to write to
		//TODO: Read in from the device that we need to write to
	}
//  else: currently loaded device is the device we're writing to
	
	// Get inode number and address of inode
	get_block(dev, mip->INODE.i_block[0], datablock);
	ino = mip->ino;
	inode = findInode(ino);
printf("iput(): mip->INODE.i_size = %d, mip->INODE.i_mode = %x,\nmip->INODE.i_block[0] = %d\n", mip->INODE.i_size, mip->INODE.i_mode, mip->INODE.i_block[0]);

	// Copy the contents of the MINODE into the location in disk
	put_block(dev, mip->INODE.i_block[0], datablock);//memcpy(inode, &(mip->INODE), sizeof(INODE));
 ///////////////////////////////////////////////////////////////////////////////
 //  This function releases a Minode[]. Since an Minode[]'s refCount indicates
 //  the number of users on this Minode[], releasing is done as follows:
 //    First, dec the refCount by 1. If (after dec) refCount > 0 ==> return;
 //    else:
 //      if Minode[].dirty == 0 ==> no need to write back, so return;
 //      Otherwise, (dirty==1) ==> must write the INODE back to disk.
 //
 //     To write an INODE back to disk:
 //     Use Minode's (dev, ino) to determine which dev and which INODE on disk,
 //   i.e. which disk block and which inode in that block.
 //   Read that block in, copy Minode's INODE into the INODE area in that block
 //   and write the block back to disk.
 // **just for simplicity, every time you close, print the reference count
 ////////////////////////////////////////////////////////////////////////////////
	return;
}

int findmyname(MINODE *parent, unsigned long myino, char *myname)
{
//   Given the parent DIR (MINODE pointer) and my inumber, this function finds 
//   the name string of myino in the parent's data block. This is similar to 
//   SERACH() mentioned above.

	// This is basically the search and isearch functions except in stead of finding a name and returning an inode
	// we are searching for an inode and returning a name

	int i = 0;
	char dpname[256];
	int result = 0;
	INODE *inode = &(parent->INODE);

	while(i < 12)
	{
		lseek(fd, inode->i_block[i] * BLOCK_SIZE, 0);
		read(fd, datablock, BLOCK_SIZE);

		dp = (DIR *)datablock;
		cp = datablock;

		while(cp < datablock + BLOCK_SIZE && dp->rec_len != 0)
		{
			strncpy(dpname, dp->name, dp->name_len);
			dpname[dp->name_len] = '\0';

			//printf("Search Ino: %d\nName: %d\n\n", (int)myino, (int)dp->inode);
			if (myino == dp->inode) // Found inode
			{
				result = 0;
				strcat(myname, "/");
				strcat(myname, dpname);
//				strcpy(myname, dpname);

printf("Test findmyname(): name[0] = %s\n", name[0]);
			
				return 0;//break;
			}

			cp += dp->rec_len;         // advance cp by rlen in bytes
			dp = (DIR *)cp;       // pull dp to the next record
        }

        i++;
	}
	return result;
}

int findino(MINODE *mip, unsigned long *myino, unsigned long *parentino)
{
	int result = 0;
	char dpname[256];
	char datablock[BLOCK_SIZE];
//  For a DIR Minode, extract the inumbers of . and .. 
//  Read in 0th data block. The inumbers are in the first two dir entries.

	get_block(dev, mip->INODE.i_block[0], datablock);
	dp = (DIR *)datablock;
	cp = datablock;

	while (cp < datablock + BLOCK_SIZE)
	{
		

		strncpy(dpname, dp->name, dp->name_len);
		dpname[dp->name_len] = '\0';
	
		if (strcmp(".", dpname)==0) // Set myino to dp->inode
		{
			//printf("found myino: %d\n", (int)dp->inode);
			*myino = dp->inode;
		}
		else if (strcmp("..", dpname)==0)
		{
			//printf("found pino: %d\n", (int)dp->inode);
			*parentino = dp->inode;
			result = 1;
			break;
		}

		cp += dp->rec_len;         // advance cp by rlen in bytes
		dp = (DIR *)cp;       // pull dp to the next record
	}

	return result;
}

unsigned long isearch(INODE *inode, char *name)
{
	int i = 0;
	char dpname[256];
	int result = 0;

	while(i < 12)
	{
		lseek(fd, inode->i_block[i] * BLOCK_SIZE, 0);
		read(fd, datablock, BLOCK_SIZE);

		dp = (DIR *)datablock;
		cp = datablock;

		while(cp < datablock + BLOCK_SIZE && dp->rec_len != 0)
		{
//			dp->name[dp->name_len] = '\0';
//			strcpy(dpname, dp->name);

			strncpy(dpname, dp->name, dp->name_len);
			dpname[dp->name_len] = '\0';
			printf("dpname = %s, length = %d\n", dpname, strlen(dpname));
			
//printf("strlen: %d\n", strlen(name));
//printf("Search: %s\nName: %s\n\n", name, dpname);
			if (strncmp(name, dpname, strlen(dpname))==0)
			{
				result = dp->inode;
				return result;
			}

			cp += dp->rec_len;         // advance cp by rlen in bytes
			dp = (DIR *)cp;       // pull dp to the next record
        }

        i++;
	}
	return result;
}

INODE *findInode(int inumber)
{
	int nblock, num;
	INODE *ino = NULL;

	nblock = iNodeBeginBlock + ((inumber - 1) / 8);
	num = ((inumber-1) % 8);

	lseek(fd, nblock*(long)BLOCK_SIZE, 0);

	if(read(fd, block, BLOCK_SIZE) > 0)
		ino = (INODE *)(&block[num*128]);
	else
		printf("Error reading inode block\n");

	return ino;
}

int quit()
{
	int i;
	for (i = 0; i < NMINODES; i++)
	{
		if (minode[i].dirty == 1)
		{
			iput(&(minode[i]));
		}
	}
	
	return 0;
}

void parseString(char *input, char *command, char *pathname)
{
	char *token;
	char cpyInput[64];
	strcpy(cpyInput, input);
	token = strtok(cpyInput, " ");
	if (token != NULL)
	{
		strcpy(command, token);
		token = strtok(NULL, " ");
	}
	else
	{
		printf("No command was found.\n");
		return;
	}
	if (token != NULL)
	{
		strcpy(pathname, token);
		token = strtok(NULL, " ");
		return;
	}
	strcpy(pathname, "");
	
	return;
}

int findCommand(char *command)
{
	char *cmdList[NUM_COMMANDS] = {"help", "ls", "cd", "mkdir", "rmdir", "pwd", "creat", "rm", "stat", "quit"};

	int i = 0;
	for(i=0;i<NUM_COMMANDS;i++)
	{
		if(strncmp(cmdList[i], command, strlen(cmdList[i]))==0)
		{
			return i;
		}
	}
		
	return -1;
}

void printInode(INODE * ip)
{
	int k;

	printf("Mode: %x\n", ip->i_mode);
	printf("UID: %u\n", ip->i_uid);
	printf("Size: %u\n", ip->i_size);

	for(k = 0; k<12;k++)
	{
		printf("Block[%d]: %u\n", k, ip->i_block[k]);
	}
	printf("\n");
	return;
}

int do_stat2(char *pathname, struct stat *stPtr)
{
	int ino;
//	int i;
	MINODE *mip;
	/* 1. Get inode of pathname into an MINODE:
		 ino = getino(&dev, pathname); // Use YOUR showBlock program; ino > 0
		 mip = iget(dev, ino);         // iget() returns a pointer to minode[]*/
	ino = getino(&dev, pathname);
	mip = iget(dev, ino);
	
	/* 3. Copy entries of INODE into stat struct;
	 As you can see, everything needed by the stat struct is in minode.INODE
	 Write C statements to copy from mip->INODE to stPtr->fields.

	   struct ext2_inode {
	i_mode;		// File mode, i_uid;		// Owner Uid
	i_size;		// Size in bytes, i_atime;	// Access time
	i_ctime;	// Creation time, i_mtime;	// Modification time
	i_dtime;	// Deletion Time, i_gid;		// Group Id
	i_links_count;	// Links count, i_blocks;	// Blocks count
		.................................
		i_block[15];    // Allocated disk blocks 
	  }
	
	struct stat {
		st_dev;      // dev, st_ino;      // ino
		st_mode;     // i_mode, st_nlink;    // i_links_count
		st_uid;      // i_uid, st_gid;      // i_gid
		st_rdev;     // IGNORE THIS, st_size;     // i_size
		st_blksize;  // 1024, st_blocks;   // i_blocks
		st_atime;    // i_atime, st_mtime;    // i_mtime
		st_ctime;    // i_ctime
	  }; */
	
	stPtr->st_dev = dev;
	stPtr->st_ino = ino;
	stPtr->st_mode = mip->INODE.i_mode;
	stPtr->st_nlink = mip->INODE.i_links_count;
	stPtr->st_uid = mip->INODE.i_uid;
	stPtr->st_gid = mip->INODE.i_gid;
	stPtr->st_size = mip->INODE.i_size;
	stPtr->st_blksize = BLOCK_SIZE;
	stPtr->st_blocks = mip->INODE.i_blocks;
	stPtr->st_atime = mip->INODE.i_atime;
	stPtr->st_mtime = mip->INODE.i_mtime;
	stPtr->st_ctime = mip->INODE.i_ctime;
	
	/* 4. Print the entries of the stat struct; */
//	TODO
	
	
	/* 5. iput(mip);      // dispose of minode; see NOTE below.
	return 0 for success; */
	iput(mip);		// dispose of minode

	return 0;
}
