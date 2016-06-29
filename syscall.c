#include "syscall.h"
 
pcb_t* pcb_loc[NUMTERMINALS] = {(pcb_t*) PCB0_LOC, NULL, NULL}; // location of the current pcb in memory
static pcb_t * prev_pcb[NUMTERMINALS] = {NULL, NULL, NULL}; // init to NULL
int32_t memoryspace[MAX_PROCESSES] = {0,0,0,0,0,0};
int32_t prog_count[NUMTERMINALS] = {0, 0, 0}; // keeps track of number of terminals

//File open jump table
int32_t (*fs_jmp_table[JMPTABLE_SIZE])() = {
        &fs_open,
        &fs_read,
        &fs_write,
        &fs_close
};
 
//stdin jump table
int32_t (*stdin_jmp_table[JMPTABLE_SIZE])() = {
        &terminal_open,
        &terminal_read,
        &terminal_write,
        &terminal_close
};
 
//stdout jump table
int32_t (*stdout_jmp_table[JMPTABLE_SIZE])() = {
        &terminal_open,
        &terminal_read,
        &terminal_write,
        &terminal_close
};
 
//directory jump table
int32_t (*dir_jmp_table[JMPTABLE_SIZE])() = {
        &dir_open,
        &dir_read,
        &dir_write,
        &dir_close
};
 
//rtc jump table
int32_t (*rtc_jmp_table[JMPTABLE_SIZE])() = {
        &rtc_open,
        &rtc_read,
        &rtc_write,
        &rtc_close
};
 
/*  halt
 *  INPUTS: status
 *  OUTPUTS: returns 0 on success    
 *  NOTES: halts the current program by switching
 *          stacks, esp, and ebp.
 *          All open files are also closed upon  
 */  
int32_t halt(uint8_t status)
{
    int8_t i;

    // close all files
    for(i = 2; i< FOPS_NUM; i++){
        close(i);
    }

    //reduce task_count and restore values to ebp/esp
    prog_count[current_terminal]--;
    memoryspace[pcb_loc[current_terminal]->pid] = 0;
    tss.ss0 = KERNEL_DS;
    pcb_loc[current_terminal]->status  = status;
    // set esp0 to correct stack position
    tss.esp0 = _8MB - (pcb_loc[current_terminal]->lastpcb_ptr->pid + 1) * MAX_STACK;
    
    asm volatile("                                  \n\
                    movl    %0, %%ebp               \n\
                    movl    %1, %%esp               \n\
                    movl    %2, %%cr3               \n\
                    jmp halt_ret                    \n\
                    "
                    :
                    : "r"(pcb_loc[current_terminal]->ebp), "r"(pcb_loc[current_terminal]->esp), "r"(pcb_loc[current_terminal]->pagedir)
                    : "memory");
    return 0;
}

/*  execute
 *  INPUTS: command: string with command and args to be executed
 *  OUTPUTS: returns ret
 *              - 0 on success  
 *              - otherwise an error code  
 *  NOTES: Checks for valid command, then separates args 
 *          and loads correct values into PCB. 
 *          Uses an IRET to jump and execute user program. 
 */   
int32_t execute(const uint8_t* command)
{
 
        uint8_t elfbuf[ELF_SIZE];       //elf buffer is bytes 0-8 of file
        uint8_t addrbuf[BUFFASIZE];     //address buffer is bytes 24-27 of file
        uint8_t argbuf[BUFFASIZE];      //argument buffer
        uint8_t prgname[BUFFASIZE];     //name of program
        uint32_t i=0;
        uint32_t pger=0;
        uint32_t namelength=0;  //length of program name       
        uint32_t arg_length=0;  //argument length      
        uint32_t entryaddr=0;   //entry point
        dentry_t curr_dentry;
 
        //check max processes
        if(prog_count[TERM_1] + prog_count[TERM_2] + prog_count[TERM_3] >= MAX_PROCESSES){
            printk("MAX PROCESSES REACHED! \n");
            return 0;
        }
                
 
        //check for invalid command
        if(command[0]=='\0' || command[0]==NULL){
                return -1;
        }
 
        //copy command into buffer
        for(i=0;i<BUFFASIZE;i++)
        {
                if(command[i]==' ' || command[i] == '\0')
                {
                        prgname[i]='\0';
                        break;
                }
                else
                {
                        prgname[i]=command[i];
                        namelength++;
                }
        }
 
        //find program name in filesystem
        if( (read_dentry_by_name(prgname, &curr_dentry))==-1 )
        {
                return -1;
        }
 
        //copy over first 4 bytes of file
        if(read_data(curr_dentry.inode, 0, elfbuf, ELF_SIZE) == -1)
        {
                return -1;
        }
 
        //magic elf checks
        if( elfbuf[0] != MAGIC0 ||
                elfbuf[1] != MAGIC1 ||
                elfbuf[2] != MAGIC2 ||
                elfbuf[3] != MAGIC3)
        {
                return -1;
        }
 
        //copy over bytes 24-27 for address
        if(read_data(curr_dentry.inode, 0, addrbuf, BUFFASIZE)== -1)
        {
                return -1;
        }      
 
        //set entry point address
        uint8_t a = addrbuf[BIT_A];
        uint8_t b = addrbuf[BIT_B];
        uint8_t c = addrbuf[BIT_C];
        uint8_t d = addrbuf[BIT_D];
        entryaddr = a | b << SHIFT | c<<(2*SHIFT)| d<<(3*SHIFT);
 
        prog_count[current_terminal]++;
        pcb_t new_pcb = pcb_init(); // create a new pcb
 
        for(pger =0; pger < MAX_PROCESSES; pger++)
         {
                if(memoryspace[pger] == 0)
                {
                        new_pcb.pid = pger;
                        memoryspace[pger] = 1;
                        break;
                }
        }

        //calculate PCB location
        pcb_t* temp = pcb_loc[current_terminal];
        pcb_loc[current_terminal] = (pcb_t*)((uint32_t)PCB0_LOC - (new_pcb.pid)*EIGHT_KB);
        
        if(prog_count[current_terminal] == 1)
            prev_pcb[current_terminal] = pcb_loc[current_terminal];
        else
            prev_pcb[current_terminal] = temp;

        setup_pcb(&new_pcb, pcb_loc[current_terminal], prev_pcb[current_terminal]);
        
        /* save relevant esp,ebps into the pcb */
        uint32_t ebpsave = 0;
        asm volatile(
                        "movl %%ebp, %0"
                        : "=a"(ebpsave)
                        :
                        : "cc" );
        pcb_loc[current_terminal]->ebp = ebpsave;
        uint32_t espsave=0;
        asm volatile(
                        "movl %%esp, %0"
                        : "=a"(espsave)
                        :
                        : "cc" );
        pcb_loc[current_terminal]->esp = espsave; 
 
        //save old pagedir into pcb
        uint32_t cr3save=0;
        asm volatile(
                        "movl %%cr3, %0"
                        : "=a"(cr3save)
                        :
                        : "cc" );
        pcb_loc[current_terminal]->pagedir = cr3save;
        
        //save args into PCB
        for(i=namelength+1;i<BUFFASIZE;i++)
        {
                if(command[i]=='\0')
                {
                        argbuf[i-namelength-1]='\0';
                        break;
                }
                else
                {
                        argbuf[i-namelength-1]=command[i];
                        pcb_loc[current_terminal]->args[i-namelength-1] = command[i]; // store argument into pcb
                        arg_length++;
                }
        }      
        //allocate new memory space depending on task count.
        if(pcb_loc[current_terminal]->pid == 0)
                prog1_init();
        else if(pcb_loc[current_terminal]->pid == 1)
                prog2_init();
        else if(pcb_loc[current_terminal]->pid == 2)
                prog3_init();
        else if(pcb_loc[current_terminal]->pid == 3)
                prog4_init();
        else if(pcb_loc[current_terminal]->pid == 4)
                prog5_init();
        else if(pcb_loc[current_terminal]->pid == 5)
                prog6_init();
       
        // copy user program into memory
        uint32_t filelen = inode_length(curr_dentry.inode);
        read_data(curr_dentry.inode, 0, (uint8_t *)PROGADDR, filelen);
 
        // save esp into tss because intel
        tss.ss0 = KERNEL_DS;
        // calculate new kernal stack pointer
        int32_t kern = 0x00800000 - (pcb_loc[current_terminal]->pid + 1) * MAX_STACK;
        tss.esp0 = kern;
        // copy "curr" into PCB, for scheduling.
        // this isn't guarenteed to be correct
        // copy new cr3 into PCB
        asm volatile("movl %%cr3, %0" 
                     : "=a"(cr3save)
                     :
                     : "cc" );

        // save kernel stack information
        pcb_loc[current_terminal]->curr_pd = cr3save;
        pcb_loc[current_terminal]->curr_esp = kern;
        pcb_loc[current_terminal]->curr_ebp = kern;
        // update terminal in which program is running to current terminal
        pcb_loc[current_terminal]->term_num = current_terminal; 
        asm volatile("              \n\
                        cli                             \n\
                        movw  %0, %%ax      \n\
                        movw %%ax, %%ds         \n\
                        pushl %0                        \n\
                        pushl %1                        \n\
                        pushfl                  \n\
                        popl %%eax                      \n\
                        orl $0x200, %%eax   \n\
                        pushl %%eax                     \n\
                        pushl %2                        \n\
                        pushl %3                        \n\
                        iret                            \n\
                        "
                        : /*no output*/
                        : "g"(USER_DS), "g"(STACKBOT), "g"(USER_CS), "g"(entryaddr) /*input*/
                        : "eax", "memory"
                        );
 
        asm volatile("  \n\
                halt_ret :      \n\
                ");
        //store val
        int8_t ret = pcb_loc[current_terminal]->status;

        // if the status is greater than 0, program ran successfully
        if(ret >0){
            ret =0; 
        }

        //update PCB loc.
        if(prog_count[current_terminal] != 0)
            pcb_loc[current_terminal] = pcb_loc[current_terminal]->lastpcb_ptr;
        return ret;
}

/*  read
 *  INPUTS: fd: file descriptor number
 *          buf: buffer to read into
 *          nbytes: number of bytes to read
 *  OUTPUTS: return value depends on function called    
 *  NOTES: wrapper for system read calls.
 *          Depending on the type of file, read jumps
 *          to different read functions
 */   
int32_t read (int32_t fd, void* buf, int32_t nbytes)
{
        if (fd < MIN_FDENTRY || fd > MAX_FDENTRY)
        {
                return -1;
        }
 
        if(pcb_loc[current_terminal]->file_desc[fd].flags == 0) // checks to see if it is in use
        {
                return -1;
        }
 
        if (fd == STDOUT_NUM) // cannot read from stdout
        {
                return -1;
        }
 
        return pcb_loc[current_terminal]->file_desc[fd].fops_ptr[CALL_READ](fd, buf, nbytes);
}

/*  write
 *  INPUTS: fd: file descriptor number
 *          buf: buffer to write into
 *          nbytes: number of bytes to write
 *  OUTPUTS: return value depends on function called    
 *  NOTES: wrapper for system write calls.
 *          Depending on the type of file, write jumps
 *          to different write functions
 */    
int32_t write (int32_t fd, const void* buf, int32_t nbytes)
{
        if(fd < MIN_FDENTRY || fd > MAX_FDENTRY)
        {
                return -1;
        }
 
        if(pcb_loc[current_terminal]->file_desc[fd].flags == 0) // checks to see if it is in use
        {
                return -1;
        }
 
        if (fd == STDIN_NUM) // cannot write to stdin
        {
                return -1;
        }
 
        return pcb_loc[current_terminal]->file_desc[fd].fops_ptr[CALL_WRITE](fd, buf, nbytes);
}

/*  open
 *  INPUTS: filename: name of file to be opened
 *  OUTPUTS: returns the file descriptor number    
 *  NOTES: Finds file in filesystem and opens it
 *          to next availible file descriptor entry
 */    
int32_t open (const uint8_t * filename)
{
        if (strlen((char *)filename) == 0)
        {
                return -1;
        }
 
        int32_t i;
        int32_t flag;
        int32_t empty;
        
        //find next open spot, skipping stdin/stdout
        for (i = FIRST_FDENTRY; i < FOPS_NUM; i++)
        {
                flag = pcb_loc[current_terminal]->file_desc[i].flags;
                if (flag == 0){
                        empty = i;
                        break;
                }
        }
 
        //no space found
        if(empty != i){
                return -1;
        }
 
        dentry_t temp_dentry;
        int check;
 
        check = read_dentry_by_name(filename, &temp_dentry);
        if (check == -1)
        {
                return -1;
        }
    
        // file descriptors should have different metadata values depending on file type
        switch(temp_dentry.type){
                case 0:
                        pcb_loc[current_terminal]->file_desc[empty].fops_ptr = rtc_jmp_table;
                        pcb_loc[current_terminal]->file_desc[empty].inode_ptr = NULL;
                        pcb_loc[current_terminal]->file_desc[empty].fops_ptr[CALL_OPEN](filename);
                        break;
                case 1:
                        pcb_loc[current_terminal]->file_desc[empty].fops_ptr = dir_jmp_table;
                        pcb_loc[current_terminal]->file_desc[empty].inode_ptr = NULL;
                        break;
                case 2:
                        pcb_loc[current_terminal]->file_desc[empty].fops_ptr = fs_jmp_table;
                        pcb_loc[current_terminal]->file_desc[empty].inode_ptr = temp_dentry.inode;
                        break;
                default: return -1;
        }
        
        // reset file position and flags to 1, signifies in use
        pcb_loc[current_terminal]->file_desc[empty].file_pos = 0;
        pcb_loc[current_terminal]->file_desc[empty].flags = 1;
 
        return empty;  
}

/*  close
 *  INPUTS: fd: file descriptor number
 *  OUTPUTS: return 0 on success , -1 otherwise
 *  NOTES: closes the file specified by fd
 *          if fd is less than 1, or file already close
 *          return -1
 */    
int32_t close (int32_t fd)
{
        if (fd < FIRST_FDENTRY || fd > MAX_FDENTRY) // do not let user close stdin (0) or stdout (1)
        {
            return -1;
        }
        
        // if attempt to close unopened file, return -1
        if(pcb_loc[current_terminal]->file_desc[fd].flags==0){
            return -1;
        }

        pcb_loc[current_terminal]->file_desc[fd].fops_ptr = NULL;
        pcb_loc[current_terminal]->file_desc[fd].inode_ptr= NULL;
        pcb_loc[current_terminal]->file_desc[fd].file_pos = NULL;
        pcb_loc[current_terminal]->file_desc[fd].flags = 0;
 
        return 0;
}

/*  getargs
 *  INPUTS: buf: buffer to read into
 *          nbytes: number of bytes to read
 *  OUTPUTS: return 0 for sucess, -1 otherwise   
 *  NOTES: copies arguments into buffer for nbytes
 *          if no argument, return -1
 */    
int32_t getargs(uint8_t * buf, int32_t nbytes)
{      
        if (nbytes == 0)
        {
                return -1;
        }
        //if no argument, return error
        if(pcb_loc[current_terminal]->args[0] == 0 || pcb_loc[current_terminal]->args[0] == '%'){
             return -1;
        }
 
        memcpy(buf, &(pcb_loc[current_terminal]->args), nbytes);
        return 0;
}

/*  vidmap
 *  INPUTS: screen_start : user pointer to map
 *  OUTPUTS: return 0 on success, -1 otherwise  
 *  NOTES: maps the user video pointer to video memory
 */    
int32_t vidmap (uint8_t ** screen_start)
{
    if (screen_start == NULL)
    {
            return -1;
    }
 
    if ( (uint32_t)(screen_start) < PROGADDR || (uint32_t)(screen_start) > (PROGADDR + _4MB)) // video memory is out of bounds
    {
            return -1;
    }
    
    *screen_start = (uint8_t*) TEXTSCREENVIDMEM;

    /* Only for scheduling
    if(pcb_loc[current_terminal]->term_num == current_terminal){
        *screen_start = (uint8_t*) TEXTSCREENVIDMEM; // maps user video memory to kernel video memory
    }
    else{
        switch(pcb_loc[current_terminal]->term_num){
            case 0: *screen_start = (uint8_t*) B_BUF_1;
            case 1: *screen_start = (uint8_t*) B_BUF_2;
            case 2: *screen_start = (uint8_t*) B_BUF_3;
        }
    }
    */
    return 0;
}

/*  set_handler
 *  INPUTS: signum: 
 *          handler: 
 *  OUTPUTS: -1 
 *  NOTES: Extra credit
 */    
int32_t set_handler(int32_t signum, void* handler)
{
        return -1;
}

/*  sigreturn
 *  INPUTS: signum: 
 *          handler: 
 *  OUTPUTS: -1 
 *  NOTES: Extra credit
 */     
int32_t sigreturn(void)
{
        return -1;
}



