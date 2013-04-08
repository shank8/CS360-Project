#include "headers/type.h"
////////////////////////////////////////////////////////
//
//	INODE Alloc/Dealloc Functions
//
////////////////////////////////////////////////////////

unsigned long ialloc(int dev)
{
	int i, j;

	// get inode Bitmap into buf[ ]

	get_block(dev, IBITMAP, block);       // assume FD, bmap block# = 4  

	for (i=0; i < BITS_PER_BLOCK; i++)
	{  // assume you know ninodes
		if (tstbit(block, i)==0)
		{    // assume you have tst_bit() function
			setbit(block, i);          // assume you have set_bit() function

			printf("open space: %d\n", i);
			put_block(dev, IBITMAP, block);   // write imap block back to disk

			// update free inode count in SUPER and GD on dev
			decFreeInodes(dev);       // assume you write this function  
			return (i+1);
		}
	}
	return 0;                     // no more FREE inodes
} 
void idealloc(int dev, unsigned long ino)
{
	int i;  

	// get inode bitmap block
	get_block(dev, IBITMAP, block);

	clearbit(block, ino-1);         // assume you have clr_bit() function 

	// write buf back
	put_block(dev, IBITMAP, block);

	// update free inode count in SUPER and GD
	incFreeInodes(dev);         // assume you write this function 
}

void decFreeInodes(dev)
{
	// We need to decrement the total free inodes in both super block and group block
	SUPER * sp;
	GD * gd;

	// SUPER BLOCK
	get_block(dev, SUPERBLOCK, block);
	sp = (SUPER *)&block[0];

	// Decrement super block
	sp->s_free_inodes_count--;

	put_block(dev, SUPERBLOCK, block);


	// GROUP BLOCK
	get_block(dev, GDBLOCK, block);

	gd = (GD *)&block[0];

	// Decrement group block
	gd->bg_free_inodes_count--;

	put_block(dev, GDBLOCK, block);

	return;
}

void incFreeInodes(dev)
{
	// We need to decrement the total free inodes in both super block and group block
	SUPER * sp;
	GD * gd;

	// SUPER BLOCK
	get_block(dev, SUPERBLOCK, block);
	sp = (SUPER *)&block[0];

	// Increment super block
	sp->s_free_inodes_count++;

	put_block(dev, SUPERBLOCK, block);


	// GROUP BLOCK
	get_block(dev, GDBLOCK, block);

	gd = (GD *)&block[0];

	// Increment group block
	gd->bg_free_inodes_count++;

	put_block(dev, GDBLOCK, block);

	return;
}

////////////////////////////////////////////////////////
//
//	BLOCK Alloc/Dealloc Functions
//
////////////////////////////////////////////////////////

unsigned long balloc(int dev)
{
	int i;

	// get inode Bitmap into buf[ ]

	get_block(dev, BBITMAP, block);       

	for (i=0; i < BITS_PER_BLOCK; i++)
	{  // assume you know ninodes
		if (tstbit(block, i)==0)
		{    // assume you have tst_bit() function
			setbit(block, i);          // assume you have set_bit() function
			put_block(dev, BBITMAP, block);   // write imap block back to disk

			// update free inode count in SUPER and GD on dev
			decFreeBlocks(dev);       // assume you write this function  
			return (i+1);
		}
	}
	return 0;                     // no more FREE inodes
} 

void bdealloc(int dev, unsigned long ino)
{
	int i;  

	// get inode bitmap block
	get_block(dev, BBITMAP, block);

	clearbit(block, ino-1);         // assume you have clr_bit() function 

	// write buf back
	put_block(dev, BBITMAP, block);

	// update free inode count in SUPER and GD
	incFreeBlocks(dev);         // assume you write this function 
}
void decFreeBlocks(dev)
{
	// We need to decrement the total free inodes in both super block and group block
	SUPER * sp;
	GD * gd;

	// SUPER BLOCK
	get_block(dev, SUPERBLOCK, block);
	sp = (SUPER *)&block[0];

	// Decrement super block
	sp->s_free_blocks_count--;

	put_block(dev, SUPERBLOCK, block);

	// GROUP BLOCK
	get_block(dev, GDBLOCK, block);

	gd = (GD *)&block[0];

	// Decrement group block
	gd->bg_free_blocks_count--;

	put_block(dev, GDBLOCK, block);

	return;
}

void incFreeBlocks(dev)
{
	// We need to decrement the total free inodes in both super block and group block
	SUPER * sp;
	GD * gd;

	// SUPER BLOCK
	get_block(dev, SUPERBLOCK, block);
	sp = (SUPER *)&block[0];

	// Increment super block
	sp->s_free_blocks_count++;

	put_block(dev, SUPERBLOCK, block);

	// GROUP BLOCK
	get_block(dev, GDBLOCK, block);

	gd = (GD *)&block[0];

	// Increment group block
	gd->bg_free_blocks_count++;

	put_block(dev, GDBLOCK, block);

	return;
}


int tstbit(char *buf, int BIT) // test BIT in buf[1024]
{
	int i, j;
	
	i = BIT / 8;
	j = BIT % 8;
	
	return buf[i] & (1 << j);
}

int setbit(char *buf, int BIT) // set BIT_th bit in char buf[1024] to 1
{
	int i, j;
	
	i = BIT / 8; 
	j = BIT % 8;
	buf[i] |= (1 << j);
	
	return 1;
}  

int clearbit(char *buf, int BIT) // clear BIT_th bit in char buf[1024] to 0
{
	int i, j;
	
	i = BIT / 8; 
	j = BIT % 8;
	buf[i] &= ~(1 << j);
	
	return 1;
}
