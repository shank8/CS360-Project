#include "headers/type.h"

GD    *gp;
SUPER *sp;
INODE *ip;
DIR   *dp;
MINODE minode[100];	//<=== 100 minodes; refCount=0 means FREE
MINODE *root;	//====>   from here on, / means minode[0].
char device[64], pathname[128];
char block[BLOCK_SIZE], datablock[BLOCK_SIZE];
char name[128][128];

int fd, n;	// file descriptor, number of names in path

SUPER *sb;
GD    *gb;
INODE *ip, *cwd;
DIR   *dp;
char *cp;
__u32 iNodeBeginBlock;

void get_device()
{
	printf("Please input a device to mount: ");
	fgets(device, 64, stdin);
	device[strlen(device)-1] = '\0';
	
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
	fd = open(device, O_RDONLY);
	if(fd < 0){
		printf("Error opening device...\n");
		return;
	}
	printf("-- Successfully opened %s for read --\n", device);
	
	// Seek to the superblock
//	lseek(fd, BLOCK_SIZE, SEEK_SET);

	// Get superblock
//	read(fd, block, BLOCK_SIZE);
	get_block(1);
	// block is now superblock

	sb = (SUPER *)&block[0];
	// Now print out some good info
	printf("#Blocks: %d\n#inodes: %d\n#BlocksPerGroup: %d\n", sb->s_blocks_count, sb->s_inodes_count, sb->s_blocks_per_group);
	printf("#inodesPerGroup: %d\n#FreeBlocks: %d\n#FreeiNodes: %d\n\n", sb->s_inodes_per_group, sb->s_free_blocks_count, sb->s_free_inodes_count);

	// NEED TO CHECK TO MAKE SURE EXT2
	if (sb->s_magic == 0xEF53)
		printf("%s is an ext2 FS device.\n", device);
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
//	lseek(fd, BLOCK_SIZE*iNodeBeginBlock, 0);
//	read(fd, block, BLOCK_SIZE);
	get_block(iNodeBeginBlock);
	
	cwd = ip = (INODE *)(block)+1;

	printInode(cwd);

	printf("WE GOT ROOT MAN!!!\n");
	
	return;
}

void printInode(INODE * ip)
{
	int k;

  printf("Mode: %u\n", ip->i_mode);
  printf("UID: %u\n", ip->i_uid);
  printf("Size: %u\n", ip->i_size);
  
  for(k = 0; k<12;k++){
	 printf("Block[%d]: %u\n", k, ip->i_block[k]);
	}
  printf("\n");
  
  return;
}
