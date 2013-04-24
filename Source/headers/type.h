/*  type.h for CS360 Project  */
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include "ext2_fs.h"
#include <libgen.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

// define shorter TYPES, save typing efforts
typedef struct ext2_group_desc	GD;
typedef struct ext2_super_block	SUPER;
typedef struct ext2_inode		INODE;
typedef struct ext2_dir_entry_2	DIR;    // need this for new version of e2fs


#define BLOCK_SIZE			1024
#define BITS_PER_BLOCK		(8*BLOCK_SIZE)
#define INODES_PER_BLOCK	(BLOCK_SIZE/sizeof(INODE))

// Block number of EXT2 FS on FD
#define SUPERBLOCK			1
#define GDBLOCK				2
#define BBITMAP				3
#define IBITMAP				4
#define INODEBLOCK			5
#define ROOT_INODE			2

// Default dir and regulsr file modes

#define DIR_MODE			0x41ED //0040777 
#define FILE_MODE			0100644
#define SYM_MODE			0120000
#define SUPER_MAGIC			0xEF53
#define SUPER_USER			0

// Proc status
#define FREE				0
#define BUSY				1
#define KILLED				2

// Table sizes
#define NMINODES			100
#define NMOUNT				10
#define NPROC				10
#define NFD					10
#define NOFT				50

#define ROOT_DEV			0
#define NUM_COMMANDS		26

// Open File Table
typedef struct Oft{
  int   mode;
  int   refCount;
  struct Minode *inodeptr;
  long  offset;
} OFT;

// PROC structure
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

// In-memory inodes structure
typedef struct Minode{
  INODE    INODE;               // disk inode
  ushort   dev;
  unsigned long ino;
  ushort   refCount;
  ushort   dirty;
  ushort   mounted;
  struct Mount *mountptr;
  char     name[128];           // name string of file
} MINODE;

// Mount Table structure
typedef struct Mount{
        int    ninodes;
        int    nblocks;
        int    dev, busy;   
        struct Minode *mounted_inode;
        char   name[256]; 
        char   mount_name[64];
} MOUNT;


// Declare externals
extern GD		*gp;
extern SUPER	*sp;
extern INODE	*ip;
extern DIR		*dp;
extern MINODE	minode[NMINODES];	//<=== 100 minodes; refCount=0 means FREE
extern MINODE	*root;	//====>   from here on, / means minode[0].
extern char	device[64], pathname[128];
extern char	block[BLOCK_SIZE], datablock[BLOCK_SIZE];
//extern char	name [128][128];
extern char	completePath[256];
extern char	newfile[256];
extern char	arg1[128];

extern int		fd, n;	// file descriptor, number of names in path

extern SUPER	*sb;
extern GD		*gb;
extern INODE	*ip, *cwd;
extern DIR		*dp;
extern char	*cp;
extern __u32	iNodeBeginBlock;
extern PROC	proc[NPROC];

extern PROC	*running;
extern int		dev;
// All function declaractions will be here

// Utility
MINODE *new_MINODE(INODE * inode, unsigned long ino, int minodeLoc, int dev);
void get_device();
void init();
int get_block(int dev, int blockNumber, char * buf);
void put_block(int dev, int blockNumber, char * buf);
void token_path(char *pathname);
unsigned long getino(int *dev, char *pathname);
unsigned long search(MINODE *mip, char *name);
unsigned long isearch(INODE *inode, char *name);
void parseString(char *input, char *arg1, char *command, char *pathname);
int quit();
int findCommand(char *command);
int fileExists(INODE * inode, char * cmpname);

// Functions
MINODE *iget(int dev, unsigned long ino, char *nodeName);
void iput(MINODE *mip);
int findmyname(MINODE *parent, unsigned long myino, char *myname);
int findino(MINODE *mip, unsigned long *myino, unsigned long *parentino);
void mount_root();
void printInode(INODE * ip);
INODE * findInode(int inumber);
void compPath(char * path);


// Cmds
int  _menu  (char *arg, char *pathname);
int  _ls    (char *arg, char *pathname);
int  _cd    (char *arg, char *pathname);
int  _mkdir (char *arg, char *pathname);
int  _rmdir (char *arg, char *pathname);
int  _pwd   (char *arg, char *pathname);
int  _creat0(char *arg, char *pathname);
int  _rm    (char *arg, char *pathname);
int  _link  (char *arg, char *pathname);
int  _touch (char *arg, char *pathname);
int  _chmod (char *arg, char *pathname);
int  _chown (char *arg, char *pathname);
int  _chgrp (char *arg, char *pathname);
int  _open  (char *arg, char *pathname);
int  _close (char *arg, char *pathname);
int  _read  (char *arg, char *pathname);
int  _write (char *arg, char *pathname);
int  _pfd   (char *arg, char *pathname);
int  _lseek (char *arg, char *pathname);
int  _cat   (char *arg, char *pathname);
int  _cp    (char *arg, char *pathname);
int  _mv    (char *arg, char *pathname);
int  __exit (char *arg, char *pathname);


// Helpers to Cmds
int rec_pwd(MINODE *wd);
int rec_complete(MINODE *wd);
int my_mkdir(MINODE *pip, char *name);
int my_creat(MINODE *pip, char *name);
void do_stat(struct stat * mystat, INODE * ino, unsigned long inode);


// Alloc/Dealloc functions
unsigned long ialloc(int dev);
void idealloc(int dev, unsigned long ino);
void decFreeInodes(int dev);
void incFreeInodes(int dev);

unsigned long balloc(int dev);
void bdealloc(int dev, unsigned long ino);
void decFreeBlocks(int dev);
void incFreeBlocks(int dev);

int tstbit(char *buf, int BIT);
int setbit(char *buf, int BIT);
int clearbit(char *buf, int BIT);
