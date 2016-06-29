#ifndef FS_H
#define FS_H

#include "types.h"

#define NAME_SIZE 32
#define MAX_DATA_NUM 1023 
#define MAX_FILES 63
#define BLOCK_SIZE 4096
#define BLOCK_RESERVE 52
#define ENTRY_RESERVE 24
#define _BLOCK_SIZE 4096

typedef struct dentry {
	uint8_t name[NAME_SIZE];
	uint32_t type; 
	uint32_t inode;
	uint8_t reserved[ENTRY_RESERVE];
} dentry_t;

typedef struct inode {
	uint32_t length;
	uint32_t blocks[MAX_DATA_NUM];
} inode_t;

typedef struct data_block
{
	uint8_t data[BLOCK_SIZE];
} data_block_t;

typedef struct boot_block
{
	uint32_t num_directory_entry;
	uint32_t num_inodes;
	uint32_t num_data_block;
	uint8_t reserved[BLOCK_RESERVE];
	dentry_t directory_entry[MAX_FILES];
} boot_block_t;

// filesystem functions for 
int32_t fs_open(const uint8_t * filename);
int32_t fs_close(int32_t fd);
int32_t fs_write(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
int32_t fs_read(int32_t fd, void* buf, int32_t nbytes);

int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
uint32_t inode_length(uint32_t inode);

int32_t dir_open(const uint8_t * filename);
int32_t dir_close(int32_t fd);
int32_t dir_write(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
int32_t dir_read(int32_t fd, void* buf, int32_t nbytes);

void setup_fs(uint32_t boot_ptr);

boot_block_t *boot;
data_block_t *data;

#endif 



