#include "headers/type.h"

MINODE minode[NMINODES];	//<=== 100 minodes; refCount=0 means FREE
MINODE *root;	//====>   from here on, / means minode[0].

char device[64], pathname[128];
char block[BLOCK_SIZE], datablock[BLOCK_SIZE];
//char name[128][128];

char completePath[256];

char arg1[128];

int fd, n;	// file descriptor, number of names in path

SUPER *sb;
GD    *gb;
INODE *ip, *cwd;
DIR   *dp;
char *cp;

PROC proc[NPROC];

PROC * running;

int dev = 0; // 0 is main device

__u32 iNodeBeginBlock;

MINODE *new_MINODE(INODE * inode, unsigned long ino, int minodeLoc, int dev)
{
	memcpy(&(minode[minodeLoc].INODE), inode, sizeof(INODE)); // This should copy the inode

	minode[minodeLoc].dev = dev;
	minode[minodeLoc].ino = ino;
	minode[minodeLoc].refCount = 1;
	minode[minodeLoc].dirty = 0;
	minode[minodeLoc].mounted = 0;
	minode[minodeLoc].mountptr = NULL;
	strcpy(minode[minodeLoc].name, "tmpName");
	
	return &minode[minodeLoc];
}

void get_device()
{
	printf("Please input a device to mount: ");
	gets(device);
	return;
}

//1. RunningPtr
//                                                        ||*********************
//     |                                                  ||
//     V        |---- PointerToCWD ------|                ||
//              |                        |                || 
//2  PROC[ ]    |    3. OFT[ ]           V 4.MINODE[ ]    ||         Disk dev
// ===========  |    ===========          ============    ||   ==================
// |nextProcPtr |    | mode    |          |  INODE   |    ||   |         INODE   
// |pid, ppid   |    |refCount |          | -------  |    ||   ================== 
// |uid         |    |MinodePtr|          | dev,ino  |    || 
// |cwd --------|    |offset   |          | refCount |    ||*********************
// |                  ===========         | dirty    |
// |fd[10]                                | mounted  |         
// | ------                     <---------| mTablePtr| 
// | ------                     |         |==========|         
// | ------                     |         |  INODE   |          
// | ------                     |         | -------  |   
// ===========                  |         |  dev,ino |   
//                              |         |  refCount|  
//                              |         |  dirty   |   
//                              |         |  mounted |   
//                              |         | mTablePtr| 
//                              |         |==========|           
//            PointAtRootInode  |                   
//                   ^          |                   
//                   |          V 
//                   |      5.   MuntTable[ ]
//                   |  ------- 0 --------- 1 ----------
//                   |  | devNumber    |            |
//                   |--| MinodePtr    |            |
//                      --------------------------------
//                      | deviceName   |            |
//                      | nblocks      |            |
//                      | ninodes      |            |
//                      ---------------------------------  
void init()
{
	/*
	typedef struct Proc{
	  int   uid;
	  int   pid;
	  int   gid;
	  int   ppid;
	  struct Proc *parent;
	  int   status;

	  struct Minode *cwd;
	  OFT   *fd[NFD];
	} PROC;
	*/
	int i = 0;

	// INIT SUPERUSER Process
	proc[SUPER_USER].uid = 0;
	proc[SUPER_USER].pid = 0;
	proc[SUPER_USER].gid = 0;
	proc[SUPER_USER].ppid = 0;
	proc[SUPER_USER].cwd = NULL;

	// INIT REG USER Process
	proc[1].uid = 1;
	proc[1].pid = 0;
	proc[1].gid = 0;
	proc[1].ppid = 0;
	proc[1].cwd = NULL;
	
	// INIT MINODES
	for(i=0;i<NMINODES;i++)
	{
		minode[i].refCount = 0;
	}



	// INIT root
	root = NULL;
	
	return;
}

void mount_root()
{
//////////////////////////////////////////////////////////////////////////////
//	load inode #2 of the root device into memory as                         //
//                                                                          //
//                    MINODE type                                           //
//                                                                          //
//                 ----------------------                                   //
//                 |  INODE (from disk) |                                   //
//                 ----------------------                                   //
//                 |  (dev, ino)=(fd,2) |                                   //
//                 |   refCount = 1     |                                   //
//                 |   dirty = 0        |                                   //
//                 ----------------------                                   //
//    typedef struct minode{                                                //
//            struct inode INODE;                                           //
//            int dev,ino;                                                  //
//            int refCount;                                                 //
//            int dirty;                                                    //
//    } MINODE;                                                             //
//                                                                          //
//    MINODE minode[100];   <=== 100 minodes; refCount=0 means FREE         //
//                                                                          //
//    load #2 inode of device into minode[0].                               //
//                                                                          //
//    let MINODE *root	//point at it ====>   from here on, / means minode[0].// 
//    let proc[0].cwd   point at it =====>  refCount = 2.                   //
//////////////////////////////////////////////////////////////////////////////
	printf("Mounting root...\n");

	// Open device for read
	fd = open(device, O_RDWR);
	if(fd < 0)
	{
		printf("Error opening device...\n");
		exit(1);
		return;
	}
	printf("-- Successfully opened %s for read --\n", device);
	
	// Seek to the superblock
//	lseek(fd, BLOCK_SIZE, SEEK_SET);

	// Get superblock
//	read(fd, block, BLOCK_SIZE);
	get_block(dev, SUPERBLOCK, block);
	// block is now superblock
	
	sb = (SUPER *)&block[0];
	// Now print out some good info
	printf("#Blocks: %d\n#inodes: %d\n#BlocksPerGroup: %d\n", sb->s_blocks_count, sb->s_inodes_count, sb->s_blocks_per_group);
	printf("#inodesPerGroup: %d\n#FreeBlocks: %d\n#FreeiNodes: %d\n\n", sb->s_inodes_per_group, sb->s_free_blocks_count, sb->s_free_inodes_count);

	// NEED TO CHECK TO MAKE SURE EXT2
	if (sb->s_magic == SUPER_MAGIC)
	{
		printf("%s is an ext2 FS device.\n", device);
	}
	else
	{
		printf("%s is not an ext2 FS device.\nExiting.\n", device);
		exit(0);
	}

	
	// Get group block
	read(fd, block, BLOCK_SIZE);
	gb = (GD *)&block[0];

	printf("iNodeBeginBlock: %d\nfreeBlocksCount: %d\nfreeInodesCount: %d\n\n", gb->bg_inode_table, gb->bg_free_blocks_count, gb->bg_free_inodes_count);
	
	iNodeBeginBlock = gb->bg_inode_table;
	printf("Begin Block: %d\n", iNodeBeginBlock);

	// Now seek to iNodeBeginBlock

	get_block(dev, iNodeBeginBlock, block);
	cwd = ip = (INODE *)(block)+1;

	root = iget(dev, ROOT_INODE, "root"); // Get the root inode from disk and put into minodes
	
	proc[SUPER_USER].cwd = proc[1].cwd = root;

	running = &proc[SUPER_USER];

	printf("Printing cwd\n");
	printInode(cwd);

	printf("Printing root\n");
	printInode(&(root->INODE));

	printf("WE GOT ROOT MAN!!!\n");
	
	return;
}

void printDir(unsigned long ino){

	/*struct ext2_dir_entry_2 {
			__u32	inode;			/* Inode number 
			__u16	rec_len;		/* Directory entry length 
			__u8	name_len;		/* Name length 
			__u8	file_type;
			char	name[EXT2_NAME_LEN];	/* File name 
		};*/

	char buf[BLOCK_SIZE];
	char name[256];
	INODE * inode;
	int i = 0;

	inode = findInode(ino);
	/*while(i<12){
		printf("block[%d] = %d\n",i, inode->i_block[i]);
		i++;
	}*/
		
	i=0;
	while(i<12)
	{
		if(inode->i_block[i] != 0)
		{
	get_block(dev, inode->i_block[i], buf);
      // Iterate through dir entries to find ..
    dp = (DIR *)buf;
    cp = buf; 
  
  	
      printf("---- DIR ENTRIES ----\n");
      while(cp < buf + BLOCK_SIZE){

      	strncpy(name, dp->name, dp->name_len);
      	name[dp->name_len] = '\0';

      	printf("inode: %d\n", (int)dp->inode);
      	printf("rec_len: %d\n", (int)dp->rec_len);
      	printf("name: %s\n\n", name);

        cp += dp->rec_len;            /* advance by rec_len */
        dp = (DIR *)cp;
      }
      	}
      i++;
     }
}

void compPath(char * path){

	strcpy(completePath, "");

	if(path[0] == '/'){
		strcpy(completePath, path);
		return;
	}else if(strcmp(path, ".")==0){
		return;
	}else if(strcmp(path, "..")==0){
		return;
	}else if(strcmp(path, "")==0){

	}else{

		if(running->cwd->ino == ROOT_INODE){
			strcat(completePath, "");
		}else{
			rec_complete(running->cwd);
		}
		strcat(completePath, "/");
		strcat(completePath, path);

	}

	printf("original: %s\n", path);
	printf("Complete Path: %s\n", completePath);
	strcpy(path, completePath);
	

	return;

}
