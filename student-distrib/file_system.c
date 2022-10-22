#include "file_system.h"

static data_block_t * data_block_ptr;  // TODO: need to set
static inode_t * inode_ptr;    // TODO: need to set
static dentry_t * dentry_ptr;  // not needed?
static boot_block_t * boot_block_ptr;  // TODO: need to set

void file_system_init(uint32_t* fs_start) {
    // point the boot_block_ptr to the head of system_start
    boot_block_ptr = (boot_block_t *) fs_start;
    // point the dentry_ptr to the head of directory entries array in bootblock
    // printf("%u ", boot_block_ptr -> dentry_count);
    // printf("%u ", boot_block_ptr -> inode_count);
    // printf("%u ", boot_block_ptr -> data_blocks_count);
    dentry_ptr = (dentry_t *) boot_block_ptr -> dir_entries;
    // point the inode_ptr to the head of inode array
    // inode and boot_block have same size, therefore add the size of boot_block_t
    // to the pointer will automatically skip the first boot_block
    inode_ptr = (inode_t *) (boot_block_ptr + 1);
    //printf("%x\n", boot_block_ptr);
    //printf("%x\n", boot_block_ptr+1);
    //printf("%u", inode_ptr -> length);
    // point the data block to the head of data block array using the similar
    // method to inode_ptr
    // data_block_ptr = (data_block_t *) (fs_start + sizeof(boot_block_ptr) +  (boot_block_ptr -> inode_count) * sizeof(inode_t));
    data_block_ptr = (data_block_t *) (inode_ptr + 64);
}

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){
    uint32_t i;
    dentry_t tmp_dentry;
    if (fname == NULL || dentry == NULL) {
        return -1;
    }
    for (i = 0; i < sizeof(boot_block_ptr -> dir_entries); i++) {
        tmp_dentry = boot_block_ptr -> dir_entries[i];
        if (strncmp(fname, tmp_dentry.filename, FILENAME_LEN)== 0) {
            strcpy(dentry -> filename, tmp_dentry.filename);
            dentry -> filetype = tmp_dentry.filetype;
            dentry -> inode_num = tmp_dentry.inode_num;
            /*dentry -> filename = tmp_dentry.filename;
            dentry -> inode_num = tmp_dentry.inode_num;*/
            //printf("\n%s found", fname);
            //printf("\n");
            break;
        }
    }
    return 0;
}

int32_t read_dentry_by_index(const uint32_t index, dentry_t* dentry){
    dentry_t tmp_dentry;
    if (index < 0 || index > 62) return -1;
    if (dentry == NULL) return -1;
    tmp_dentry = boot_block_ptr -> dir_entries[index];
    strcpy(dentry -> filename, tmp_dentry.filename);
    dentry -> filetype = tmp_dentry.filetype;
    dentry -> inode_num = tmp_dentry.inode_num;
    return 0;
}

int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
    // error checking
    if (inode < 0 || inode > 62) return -1;
    if (buf == NULL) return -1;

    uint32_t inode_startblk_idx = offset / 4096;     // calculate which block to start with
    uint32_t startblk_idx = offset % 4096;           // calculate the start byte within the start block
    uint32_t inode_endblk_idx;
    uint32_t endblk_idx;

    // calculate which block to end with 
    if (length > inode_ptr[inode].length)
        inode_endblk_idx = inode_ptr[inode].length / 4096;
    else
        inode_endblk_idx = (length + offset) / 4096;

    endblk_idx = (inode_ptr[inode].length + offset) % 4096;      // calculate the end byte within the end block
    
    uint32_t idx, i;
    uint32_t buf_idx = 0;   // stores where we are at in the buffer
    uint32_t block_idx;     // index to the data blocks
    uint32_t bytes_read = 0;    // number of bytes read
    inode_t curr_inode = inode_ptr[inode];
    
    // loop through each data_block_num entry in the current inode
    for (idx = inode_startblk_idx; idx <= inode_endblk_idx; ++idx){

        // if it's the first block
        if (idx == inode_startblk_idx){ 
            for (i = startblk_idx; i < 4096; ++i){
                block_idx = curr_inode.data_block_num[idx];
                // block_idx error checking
                if (block_idx < 0)  // TODO CHECK UPPER BOUND
                    return -1;
                // copy data of the first block (from startblk_idx to the end of this block) to buffer 
                buf[buf_idx] = data_block_ptr[block_idx].entry[i];  // SOMETHING WRONG HERE
                buf_idx++;
                bytes_read++;
            }
        }

        // if it's the last block
        else if (idx == inode_endblk_idx){  
            // copy the data of the last block (from 0 to endblk_idx) to buffer
            for (i = 0; i < endblk_idx; ++i){
                block_idx = curr_inode.data_block_num[idx];
                // block_idx error checking
                if (block_idx < 0)  // TODO CHECK UPPER BOUND
                    return -1;
                buf[buf_idx] = data_block_ptr[block_idx].entry[i];
                buf_idx++;
                bytes_read++;
            }
        }

        // if it's not the first or last block, copy the entire block
        else {  
            for (i = 0; i < 4096; ++i){
                block_idx = curr_inode.data_block_num[idx];
                // block_idx error checking
                if (block_idx < 0)  // TODO CHECK UPPER BOUND
                    return -1;
                buf[buf_idx] = data_block_ptr[block_idx].entry[i];
                buf_idx++;
                bytes_read++;
            }
        }
    }
    return bytes_read;
}

int file_open(uint8_t* fname) {
    dentry_t tmp_dentry;
    //inode_t tmp_inode;

    read_dentry_by_name (fname, &tmp_dentry);

    // if (read_dentry_by_name (fname, &tmp_dentry) == 0) {
    //     if (strncmp(fname, tmp_dentry.filename, FILENAME_LEN) != 0) {
    //         printf ("\nnot found");
    //         printf ("\n");
    //     } else {
    //         printf ("%s, ", tmp_dentry.filename);
    //         printf ("%u, ", tmp_dentry.filetype);
    //         printf ("%u", inode_ptr[tmp_dentry.inode_num].length);
    //     }
    //     //printf ("found");
    // }
    //

    //tmp_inode = tmp_dentry.
    return 0;
}

void file_read(uint8_t* fname) {
    dentry_t tmp_dentry;
    uint8_t read_buf[4096];
    unsigned i;
    read_dentry_by_name (fname, &tmp_dentry);
    read_data (tmp_dentry.inode_num, 0, read_buf, 187);
    // clear();

    //bug here, need to shift three lines
    printf("\n");
    printf("\n");
    printf("\n");

    for (i = 0; i < 187; i++) {
        printf("%c", read_buf[i]);
    }
    
}

int file_write() {
    return -1;
}

int file_close() {
    //inode_ptr = (inode_t *) (fs_start + sizeof(boot_block_t));
    return 0;
}

int dir_open() {
    return 0;
}

void dir_read(int32_t fd, void* buf, int32_t nbytes) {
    // ignoring fd, nbytes for 3.2
    /*uint32_t idx;
    (uint8_t*) buf;
    // need change for 3.3 
    
    while (){
        dentry_t curr_dentry = boot_block_ptr->dir_entries[idx];
        if (curr_dentry.filename == '\0') return 0; // 0 byte is read for empty entries
        
    }*/
}

int dir_write() {
    return -1;
}

int dir_close() {
    return 0;
}

void files_ls(){
    uint32_t idx;
    uint8_t fname;
    uint8_t ftype, inode_num;
    dentry_t tmp_dentry;
    for (idx = 0; idx < boot_block_ptr -> dentry_count; idx++){
        /*strcpy (name, boot_block_ptr->dir_entries[idx].filename);
        type = boot_block_ptr->dir_entries[idx].filetype;
        inode_num = boot_block_ptr->dir_entries[idx].inode_num;
        if (name == '\0') continue;
        printf("file_name: %s, file_type: %d, file_size: %d \n", name, type, inode_num);*/
        read_dentry_by_index(idx, &tmp_dentry);
        printf("file_name: %s, file_type: %d, file_size: %d \n", 
        tmp_dentry.filename, tmp_dentry.filetype, inode_ptr[tmp_dentry.inode_num].length);    
    }
}
