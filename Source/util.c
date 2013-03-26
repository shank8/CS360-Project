#include "headers/type.h"

// lseek and read done within this function, stores into global buf
int get_block(int blockNumber)
{
	lseek(fd,(long)BLOCK_SIZE*blockNumber, 0);
	read(fd, block, BLOCK_SIZE);
	
	return 0;
}

void put_block(int blockNumber)
{
   lseek(fd, (long)BLOCK_SIZE*blockNumber, 0);
   write(fd, block, BLOCK_SIZE);
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
		printf("token%d = %s\n", n, token);
		n++;
		token = strtok(NULL, "/");
	}
    printf("n = %d\n", n);
	
	return;
}

unsigned long getino(int *dev, char *pathname)
{
	unsigned long result;
//  This is the most important function of the project. It converts
//  a pathname, such as /a/b/c/d OR x/y/z, into its (dev, inumber), i.e. 
//  the returned value is its inumber and dev is its dev number.
//
//  NOTE: while traversing a pathname, the starting dev number is the dev of / 
//  OR the dev of (running's) CWD. If you have only one device, the dev number 
//  will not change. But when crossing MOUNTed file systems (for Level-3), the
//  dev number may change. The dev parameter is used to keep the CURRENT dev 
//  number.
//
//  This function depends on search(), which searches an DIR's data block for 
//  a component name, and returns the component's inmuber. Modify *dev if the
//  dev number changes.
//  
//  You have already done these in showblock.c
	
	return result;
}

unsigned long search(MINODE *mip, char *name)
{
	unsigned long result;
//   This function searches the data blocks of a DIR inode (inside an Minode[])
//   for name. You may assume DIRECT data blocks only.
	
	return result;
}

MINODE *iget(int dev, unsigned long ino)
{
	MINODE *tmpMINode;
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

	return tmpMINode;
}

void iput(MINODE *mip)
{
//  This function releases a Minode[]. Since an Minode[]'s refCount indicates
//  the number of users on this Minode[], releasing is done as follows:
//    First, dec the refCount by 1. If (after dec) refCount > 0 ==> return;
//    else:
//      if Minode[].dirty == 0 ==> no need to write back, so return;
//      Otherwise, (dirty==1) ==> must write the INODE back to disk.
   
//  To write an INODE back to disk:
//     Use Minode's (dev, ino) to determine which dev and which INODE on disk,
//  i.e. which disk block and which inode in that block.
//  Read that block in, copy Minode's INODE into the INODE area in that block
//  and write the block back to disk.
	
	return;
}

int findmyname(MINODE *parent, unsigned long myino, char *myname)
{
	int result;
//   Given the parent DIR (MINODE pointer) and my inumber, this function finds 
//   the name string of myino in the parent's data block. This is similar to 
//   SERACH() mentioned above.
	
	return result;
}

int findino(MINODE *mip, unsigned long *myino, unsigned long *parentino)
{
	int result;
//  For a DIR Minode, extract the inumbers of . and .. 
//  Read in 0th data block. The inumbers are in the first two dir entries.
	
	return result;
}
