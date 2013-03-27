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

	// I DO NOT KNOW WHAT int * dev is ???

	char * tok;
	unsigned long inumber = 0;
	INODE * next = ip;

	printf("Pathname: %s\n", pathname);

	tok = strtok(pathname, "/");

	while(tok != NULL){
		printf("tok: %s\n", tok);
		inumber = isearch(next, tok);

		if(inumber == 0){
			printf("Path cannot be found\n");
			return inumber;
		}

		next = findInode(inumber);
		printf("Path found!\n");
		tok = strtok(NULL, "/");
	}

	printf("Search done\n");

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



unsigned long isearch(INODE * inode, char * name){

	int i = 0, k = 0;
	char dpname[256];
	int result = 0;

	while(i < 12){
		lseek(fd, inode->i_block[i] * BLOCK_SIZE, 0);
		read(fd, datablock, BLOCK_SIZE);

		dp = (DIR *)datablock;
		cp = datablock;

		while(cp < datablock + BLOCK_SIZE && dp->rec_len != 0){
			k = 0;
			
			dp->name[dp->name_len] = '\0';

			strcpy(dpname, dp->name);

			printf("Search: %s\nName: %s\n\n", name, dpname);
           if (strcmp(name, dpname)==0)
           {

              result = dp->inode;
           }
          
           cp += dp->rec_len;         // advance cp by rlen in bytes
           dp = (DIR *)cp;       // pull dp to the next record
        }

        i++;

	}
	return result;
}

INODE * findInode(int inumber){

	int nblock, num;
	INODE * ino = NULL;

	nblock = iNodeBeginBlock + ((inumber - 1) / 8);
	num = ((inumber-1) % 8);

	lseek(fd, nblock*(long)BLOCK_SIZE, 0);

	if(read(fd, block, BLOCK_SIZE) > 0){
		ino = (INODE *)(&block[num*128]);
	}else{
		printf("Error reading inode block\n");
	}

	return ino;
}
