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

	char * tok;
	unsigned long inumber = 0;
	INODE * next = &root->INODE;

	

	tok = strtok(pathname, "/");
	if(tok==NULL)
	{
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
		
	printInode(tmpInode);

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
	int dev = 0, ino;
	INODE * inode = NULL;

	mip->refCount--;
	printf("name: %s\n", mip->name);
	printf("refCount: %d\n", mip->refCount);
	printf("dirty: %d\n", mip->dirty);
	printf("ino: %d\n", mip->ino);
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
	ino = mip->ino;
	inode = findInode(ino);


	// Copy the contents of the MINODE into the location in disk
	memcpy(inode, &(mip->INODE), sizeof(INODE));
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
	printf("test\n\n");
	return;
}

int findmyname(MINODE *parent, unsigned long myino, char *myname)
{
//   Given the parent DIR (MINODE pointer) and my inumber, this function finds 
//   the name string of myino in the parent's data block. This is similar to 
//   SERACH() mentioned above.

	// This is basically the search and isearch functions except in stead of finding a name and returning an inode
	// we are searching for an inode and returning a name

	int i = 0, k = 0; // do we need the variable k?
	char dpname[256];
	int result = 0;
	INODE * inode = &(parent->INODE);

	while(i < 12)
	{
		lseek(fd, inode->i_block[i] * BLOCK_SIZE, 0);
		read(fd, datablock, BLOCK_SIZE);

		dp = (DIR *)datablock;
		cp = datablock;

		while(cp < datablock + BLOCK_SIZE && dp->rec_len != 0)
		{
			k = 0;

			dp->name[dp->name_len] = '\0';

			strcpy(dpname, dp->name);

			printf("Search: %s\nName: %s\n\n", (char *)name, (char *)dpname);
			if (myino == dp->inode) // Found inode
			{
				result = 1;
				myname = (char*)(&dp->name);
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
//  For a DIR Minode, extract the inumbers of . and .. 
//  Read in 0th data block. The inumbers are in the first two dir entries.

	get_block(dev, mip->INODE.i_block[0], block);
	dp = (DIR *)datablock;
	cp = datablock;

	while (cp < datablock + BLOCK_SIZE && dp->rec_len != 0)
	{
		dp->name[dp->name_len] = '\0';

		strcpy(dpname, dp->name);

		printf("Search: %s\nName: %s\n\n", (char*)name, (char *)dpname);

		if (strcmp(".", dpname)==0) // Set myino to dp->inode
		{
			*myino = dp->inode;
		}
		else if (strcmp("..", dpname)==0)
		{
			*parentino = dp->inode;
			result = 1;
			break;
		}

		cp += dp->rec_len;         // advance cp by rlen in bytes
		dp = (DIR *)cp;       // pull dp to the next record
	}

	return result;
}

unsigned long isearch(INODE * inode, char * name)
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

			dp->name[dp->name_len] = '\0';

			strcpy(dpname, dp->name);
			printf("strlen: %d", strlen(name));
			printf("Search: %s\nName: %s\n\n", name, dpname);
			if (strncmp(name, dpname, strlen(dpname))==0)
			{
				printf("ladjflasdfj");
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
	INODE * ino = NULL;

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
	if ( (strncmp(command, "menu", 4) == 0) || (strncmp(command, "help", 4) == 0) )
		return 0;
	if (strncmp(command, "ls", 2) == 0)
		return 1;
	if (strncmp(command, "cd", 2) == 0)
		return 2;
	if (strncmp(command, "mkdir", 5) == 0)
		return 3;
	if (strncmp(command, "rmdir", 5) == 0)
		return 4;
	if (strncmp(command, "pwd", 3) == 0)
		return 5;
	if (strncmp(command, "creat", 5) == 0)
		return 6;
	if (strncmp(command, "rm", 2) == 0)
		return 7;
	if ( (strncmp(command, "quit", 4) == 0) || (strncmp(command, "exit", 4) == 0) )
		return 8;
		
	return -1;
}


void printInode(INODE * ip)
{
	int k;

	printf("Mode: %u\n", ip->i_mode);
	printf("UID: %u\n", ip->i_uid);
	printf("Size: %u\n", ip->i_size);

	for(k = 0; k<12;k++)
	{
		printf("Block[%d]: %u\n", k, ip->i_block[k]);
	}
	printf("\n");
	return;
}
