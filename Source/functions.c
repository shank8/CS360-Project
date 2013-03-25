#include "headers/type.h"

char device[64], pathname[128];
char block[BLOCK_SIZE], datablock[BLOCK_SIZE];

int fd;

SUPER *sb;
GD    *gb;
INODE *ip, *cwd;
DIR   *dp;

char * cp;
__u32 iNodeBeginBlock;

void get_device(){
	printf("Please input a device to mount: ");
	gets(device);
}

void mount_root(){

	printf("Mounting root...\n");

	// Open device for read
	fd = open(device, O_RDONLY);
	if(fd < 0){
		printf("Error opening device...\n");
		return;
	}
	printf("-- Successfully opened device for read --\n");
	
	// Seek to the superblock
	lseek(fd, BLOCK_SIZE, SEEK_SET);

	// Get superblock
	read(fd, block, BLOCK_SIZE);
	// block is now superblock

	sb = (SUPER *)&block[0];
	// Now print out some good info
	printf("#Blocks: %d\n#inodes: %d\n#BlocksPerGroup: %d\n", sb->s_blocks_count, sb->s_inodes_count, sb->s_blocks_per_group);
	printf("#inodesPerGroup: %d\n#FreeBlocks: %d\n#FreeiNodes: %d\n\n", sb->s_inodes_per_group, sb->s_free_blocks_count, sb->s_free_inodes_count);

	// NEED TO CHECK TO MAKE SURE EXT2


	// Get group block
	read(fd, block, BLOCK_SIZE);
	gb = (GD *)&block[0];

	printf("iNodeBeginBlock: %d\nfreeBlocksCount: %d\nfreeInodesCount: %d\n\n", gb->bg_inode_table, gb->bg_free_blocks_count, gb->bg_free_inodes_count);
	
	iNodeBeginBlock = gb->bg_inode_table;
	printf("Begin Block: %d\n", iNodeBeginBlock);

	// Now seek to iNodeBeginBlock
	lseek(fd, BLOCK_SIZE*iNodeBeginBlock, 0);

	read(fd, block, BLOCK_SIZE);

	
	cwd = ip = (INODE *)(block)+1;

	printInode(cwd);

	printf("WE GOT ROOT MAN!!!\n");
}

void printInode(INODE * ip){

	int k;

  printf("Mode: %u\n", ip->i_mode);
  printf("UID: %u\n", ip->i_uid);
  printf("Size: %u\n", ip->i_size);
  
  for(k = 0; k<12;k++){
	 printf("Block[%d]: %u\n", k, ip->i_block[k]);
	}
  printf("\n");
}