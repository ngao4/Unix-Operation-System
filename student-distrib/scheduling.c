#include "scheduling.h"
/*Global variable*/

uint32_t running_term = 0; // Term: F1, F2, F3
uint32_t schedule_counter = 1;

/* 
 *  void scheduler()
 *  DESCRIPTION: schedule between three process, set up correct program image and page usage
 *  INPUTS: none, this function is triggered by PIT, time slice is 10ms
 *  RETURN VALUE:none
 */
void scheduler(){
    /* go to the next terminal using Round Robin */
    running_term = (running_term % 3) + 1 ;
    /* start executing three shells before any other processes */
    if ( pcb_counter[0] == NULL || pcb_counter[1] == NULL || pcb_counter[2] == NULL){
        register uint32_t current_ebp asm("ebp");
        register uint32_t current_esp asm("esp");
        current_pcb_pointer->current_ebp = current_ebp;
        current_pcb_pointer->current_esp = current_esp;
        sys_execute("shell");
    } 
    /* Map the text-mode video memory to current terminal's video page in user space */
    page_directory[VIDMAP_PAGE_INDEX].pd_kb.val = ((uint32_t)vid_page_table) | VIDMAP_MAGIC;    // VIDMAP_MAGIC == PD's present, R_W, U_S 
    vid_page_table[VIDPAGE_IDX].present = 1;
    vid_page_table[VIDPAGE_IDX].read_write = 1;
    vid_page_table[VIDPAGE_IDX].user_supervisor = 1;  
    //Tony: this part is necessary! with some reason, we need to make sure this page is setup correctly 
    // otherwise there will be page fault

    // choose which video page should we map to (determined by the currently displaying terminal)
    /*sys_vidmap rely on this part*/
       if (display_term == running_term)
        vid_page_table[VIDPAGE_IDX].base_addr = (VIDEO_MEMORY >> PT_SHIFT);        // B8000 >> 12 
    else    // map to the correct video page according to currently running terminal
        vid_page_table[VIDPAGE_IDX].base_addr = ((VIDEO_MEMORY + VIDPAGE_SIZE*running_term) >> PT_SHIFT);

    /* save current ebp and esp */
    register uint32_t current_ebp asm("ebp");
    register uint32_t current_esp asm("esp");
    current_pcb_pointer->current_ebp = current_ebp;
    current_pcb_pointer->current_esp = current_esp;
    /*Load next pcb*/
    struct pcb_t* next_pcb = terminal[running_term].running_pcb;
    current_pcb_pointer = next_pcb;
    /* restore TSS of the next process */
    tss.esp0 = (KERNEL_BOTTOM - PROCESS_SIZE * (current_pcb_pointer-> pid - 1) - 4 );
    tss.ss0 = KERNEL_DS;
    /* Map program image to the next process' address */
    page_directory[PROG_IMG].pd_mb.val = 0;   // clear the entry
    page_directory[PROG_IMG].pd_mb.user_supervisor = 1;
    page_directory[PROG_IMG].pd_mb.present = 1;    
    page_directory[PROG_IMG].pd_mb.read_write = 1;
    page_directory[PROG_IMG].pd_mb.page_size = 1;  // initialize 4mb page 
    page_directory[PROG_IMG].pd_mb.base_addr = (KERNEL_POSITION) + next_pcb->pid; // provide the address of the next program
   



    /* flush TLB (OSdev) */
    asm volatile(
        "movl %%cr3, %%eax;"
        "movl %%eax, %%cr3;"
        :
        :
        : "eax", "cc"
    );
    /* switch to the next process' esp & ebp */
            asm volatile (
            "movl %0, %%esp;"
            "movl %1, %%ebp;" 
            :
            : "r" (current_pcb_pointer->current_esp), "r" (current_pcb_pointer->current_ebp)
            : "ebp", "esp"  
        );
}

