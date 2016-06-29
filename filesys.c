/*
 * 	COMMENTED 
 *  MAGICKED 
 */ 

#include "filesys.h"
#include "lib.h"
#include "pcb.h"

/* fs utilities */

/*	read_data
 *	inputs: inode 	: inode number
 *			offset	: position in file to begin reading
 *			buf 	: buffer to read into
 *			length 	: number of bytes to read into
 *	outputs: -1 for any error
 *			  0 if reached end of file
 * 			  otherwise, returns number of bytes read
 *	notes: This reads length bytes into a buffer starting at an offset for
 * 			a given inode file number in the read-only filesystem.
 */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
	
	inode_t* inode_ptr;
	inode_ptr = (inode_t*)(&(boot[inode + 1]));
	uint32_t filelength = inode_ptr->length; 
	
	// Arg checking
	if(	length==0 || 
		buf == NULL	||
		inode > MAX_DATA_NUM){
		return -1; 
	}

	// end of file reached if offset is larger than file
	if(offset > filelength){
		return 0;
	}

	/* If the offset + length is longer than file, 
		set the length to be read to filelength */
	if( (offset + length) > filelength) {
		length = filelength;
	}

	uint32_t maxblocks = boot->num_data_block;
	uint32_t position = offset; 
	uint32_t blocknum = position/BLOCK_SIZE;
	uint32_t blockoff = position%BLOCK_SIZE;
	uint32_t bytesread= 0; 

	/* Read data into buffer a single byte at a time until
		either end of file or length is reached*/
	while(bytesread < length){
		
		// blocknum specifies the current block
		blocknum = position/BLOCK_SIZE;
		// blockoff is the current position in the block
		blockoff = position%BLOCK_SIZE;

		/* if either block or position reaches over end of file,
			break, but do not return 0. */
		if(inode_ptr->blocks[blocknum] >= maxblocks){
			break;
		}
		if(position > filelength){
			break; 
		}

		// copy over data one byte at a time
		memcpy(buf+bytesread, &(data[inode_ptr->blocks[blocknum]].data[blockoff]), 1);
		bytesread++;
		position++;
	}

	return bytesread;
}

/*	read_dentry_by_name
 *	inputs: fname 	: filename to be found
 *			dentry 	: dentry to be written to
 *	outputs: -1 if file not found
 *			  0 if file found
 *	notes: Finds the file in the filesystem and copies 
 *			over dentry metadata into given dentry
 */
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){
	
	uint32_t i;
	// Go through all the files one by one and find filename match
	for (i = 0; i < MAX_FILES; i++) {
		if (strncmp((int8_t*) (boot->directory_entry[i].name), (int8_t*) (fname), NAME_SIZE - 1) == 0) {
			
			*dentry = boot->directory_entry[i];

			strcpy((int8_t*)(dentry->name), (int8_t*) (boot->directory_entry[i].name));
			dentry->type = boot->directory_entry[i].type;
			dentry->inode = boot->directory_entry[i].inode;
		
			return 0;
		}
	}

	// Failed to find file
	return -1;
}

/*	read_dentry_by_index
 *	inputs: index 	: inode num in file directory 
 *			dentry 	: dentry to be written to
 *	outputs: -1 if index is invalid
 *			  0 if copy was successful
 *	notes: Copies dentry metadata to given dentry
 */
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry){
	
	//arg checking 
	if (index > boot->num_inodes)
	{
		return -1;
	}

	*dentry = boot->directory_entry[index];

	strcpy((int8_t*)(dentry->name), (int8_t*) (boot->directory_entry[index].name));
	dentry->type = boot->directory_entry[index].type;
	dentry->inode = boot->directory_entry[index].inode;

	return 0;
}

/* helper defs */

/*	setup_fs
 *	inputs: boot_ptr: pointer to filesystem
 *	outputs: none
 *	notes: maps filesystem boot block and data block to 
 * 			correct position given a boot pointer
 */
void setup_fs(uint32_t boot_ptr)
{
	boot = (boot_block_t*)boot_ptr;
	
	// data is 31*4kb away from beginning of boot block. (30 inodes+1) 
	data = (data_block_t*)(boot_ptr + BLOCK_SIZE + boot->num_inodes * BLOCK_SIZE); 
}

/*	inode_length
 *	inputs: inode : inode number in filesystem
 *	outputs: length of file 
 *	notes: This returns the length of a file given its inode number
 */
uint32_t inode_length(uint32_t inode){
	inode_t* inode_ptr;
	inode_ptr = (inode_t*)(&(boot[inode + 1]));
	return inode_ptr->length; 
}

/* fs syscalls */

/*	The following functions are not defined in our current filesystem
 *	Only fs_read is utilized. 	
 */


/*	fs_open(const uint8_t * filename);
 *	inputs: not utilized
 *	outputs: 0
 *	notes: 	Filesystem open system call for files
 *			Is not defined in our current filesystem.
 */
int32_t fs_open(const uint8_t * filename){
	return 0;
}

/*	fs_write(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
 *	inputs: not utilized
 *	outputs: -1
 *	notes: 	Filesystem write system call for files
 *			Is not defined in our current filesystem (read-only filesystem).
 */
int32_t fs_write(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
	return -1;
}

/*	fs_close(int32_t fd);
 *	inputs: not utilized
 *	outputs: 0
 *	notes: 	Filesystem close system call for files
 *			Is not defined in our current filesystem.
 */
int32_t fs_close(int32_t fd){
	return 0;
}

/*	fs_read
 *	inputs: fd 	: file descriptor
 *			buf : buffer to write into
 * 			nbytes: number of bytes to write
 *	outputs: -1 on failure
 *			otherwise, number of bytes read
 *	notes: Reads nbytes into a buffer given a file
 * 			descriptor. 
 */
int32_t fs_read(int32_t fd, void* buf, int32_t nbytes) {
	
	dentry_t curr_dentry;
	int32_t check;
	int32_t inum; 
	int32_t readpos; 
	int32_t flent; 

	if(!buf){
		return -1;
	}

	inum = pcb_loc[current_terminal]->file_desc[fd].inode_ptr; // we need to fix this because its a temp fa 
	readpos = pcb_loc[current_terminal]->file_desc[fd].file_pos; 
	flent = inode_length(inum);
	inode_t* inode_ptr;
	inode_ptr = (inode_t*)(&(boot[inum + 1]));
	
	// verify file existence 
	check = read_dentry_by_index(inum, &curr_dentry);
	if (check == -1){
		return -1;
	}

	// read data into buffer
	check = read_data(inum, readpos, buf, nbytes);
	if(check == -1){
		return -1; 
	}
	//update file position
	pcb_loc[current_terminal]->file_desc[fd].file_pos += check; // we need to fix this because its a temp fa 
	
	//returns number of read bytes
	return check;
}

/* directory syscalls */

/*	dir_open(const uint8_t * filename);
 *	inputs: not utilized
 *	outputs: -1
 *	notes: 	Filesystem open system call for directories
 *			Is not defined in our current filesystem.
 */
int32_t dir_open(const uint8_t * filename){
	return -1; 
}

/*	dir_write(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
 *	inputs: not utilized
 *	outputs: -1
 *	notes: 	Filesystem write system call for files
 *			Is not defined in our current filesystem (read-only filesystem).
 */
int32_t dir_write(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
	return -1;
}

/*	dir_close(int32_t fd);
 *	inputs: not utilized
 *	outputs: -1
 *	notes: 	Filesystem close system call for files
 *			Is not defined in our current filesystem.
 */
int32_t dir_close(int32_t fd){
	return -1;
}

/*	dir_read
 *	inputs: fd 	: file descriptor
 *			buf : buffer to write into
 * 			nbytes: number of bytes to write
 *	outputs: -1 on failure
 *			0 signfies that directory has been completely read	
 *			otherwise, number of bytes read
 *	notes: Same as fs_read, except only file names are read
 * 			into buffer
 */
int32_t dir_read(int32_t fd, void* buf, int32_t nbytes){
	
	dentry_t curr_dentry;
	int32_t check;
	int32_t readpos; 
	
	if(!buf){
		return -1;
	}

	readpos = pcb_loc[current_terminal]->file_desc[fd].file_pos; 
	check = read_dentry_by_index(readpos, &curr_dentry);
	
	/* if either name is null, or file is not found, return 0
		to signal that directory has finished reading*/
	if(check == -1){
		return 0;
	}
	if(curr_dentry.name[0] == '\0'){
		return 0;
	}
	
	// make sure nbytes is size of string length
	if(strlen((char *)curr_dentry.name) < nbytes){
		nbytes = strlen((char *)curr_dentry.name);
	}
	if(nbytes>NAME_SIZE){
		nbytes = NAME_SIZE;
	}
	
	memcpy(buf, &(curr_dentry.name), nbytes);
	
	//update file position
	readpos++;
	//returns number of read bytes
	pcb_loc[current_terminal]->file_desc[fd].file_pos = readpos;

	return nbytes;
}
