# Todo list of SysCall

1. Jerry: File system's read write open close
data structure is of problem

write file & dir different?
Should be same
2. 
RTC first for testing

# syscall_link.h - Assembly linkage for device and idt
.data

.text

.globl syscall_handler
# return value: none
syscall_handler:                            \
    pushal                                  ;\
    pushfl                                  ;\
	movl	8(%esp), %ebx   #  cmd -> ebx   ;\
	cmpl	$0, %ebx                        ;\
# if ebx 0                  

    call *jump_table(,%ebx,4)               ;\
    popfl                                   ;\
    popal                                   ;\

    iret



jump_table:
		.long sys_open, sys_close, sys_read, sys_write


# 0 -> dummy jump? sequence matters!



; movl	8(%esp), %ebx   #  cmd -> ebx   ;\
	; cmpl	$0, %ebx            

3. how to connect the fd entry and pcb?


4. 清理keyboard 的open close

5. 
|| filename < USER_SPACE_START || filename > USER_SPACE_END - 32
        for (i = 0; i < 6; i++) { // is this for loop necessary?
            if (pcb_1.fd_entry[i].flag == 0) {
                // set function operation table pointer
                if (find_next_fd() < 0) {
                    return -1;
                }
                idx = find_next_fd();
                
                new_fd_entry.file_pos = fop_table[file_type];

                pcb_1.fd_entry[idx] = new_fd_entry;

                break;
            }
            // if no fd left,  what to do ?
        }


6. todo

    asm volatile(
        "xorl %%eax, %%eax;"
        "movl %0, %%eax;"
        "movw %%ax, %%ds;"
        "pushl %0;" 
        "pushl %1;"
        "orl  $0x200, %%eflags;"
        "pushfl;"
        "pushl %2;"
        "pushl %3;"
        "IRET;"
        :
        : "r" (user_data_segment), "r" (esp), "r" (user_code_segment), "r" (eip)
        : "cc", "memory", "eax"
    );
->

    asm volatile(
        "pushl %0;" 
        "pushl %1;"
        "pushfl;"
        "pushl %2;"
        "pushl %3;"
        "IRET;"
        :
        : "r" (user_data_segment), "r" (esp), "r" (user_code_segment), "r" (eip)
        : "cc", "memory", "eax"
    );
is enough for syscall 

7. where is the terminal write and read 
-> in execute