/********************************************************
 * Filename: hal/linux/context.c
 *
 * Author: Jiyong Park, RTOSLab. SNU
 * Modified by: Seongsoo Hong on 03/31/24
 *
 * Description: Context management (define, create, switch)
 ********************************************************/

#include <core/eos.h>
#include "emulator_asm.h"

typedef struct _os_context {
    // To be filled by students: Project 2
    // Emulate the stack of the interrupted task right after pushal instruciton
    /* low address */
    /* high address */	
    //addr_t stack_base;      //base stack pointer
    //int32u_t stack_size;    //stack size
    
    //registers
    //LOMEM
    int32u_t edi;
    int32u_t esi;
    int32u_t ebp;
    int32u_t esp;
    int32u_t ebx;
    int32u_t edx;
    int32u_t ecx;
    int32u_t eax;
    int32u_t eflags;        //eflags
    int32u_t eip;           //program counter
    //HIMEM

} _os_context_t;


void print_context(addr_t context)
{
    if(context == NULL)
        return;

    _os_context_t *ctx = (_os_context_t *)context;

    // To be filled by students if needed: Project 2
    //PRINT("stack_base   =0x%x\n", ctx->stack_base);
    //PRINT("stack size   =%u\n", ctx->stack_size);
    PRINT("==========================\n");
    PRINT("pointer  =0x%x\n", ctx     );   
    PRINT("edi      =0x%x\n", ctx->edi);
    PRINT("esi      =0x%x\n", ctx->esi);
    PRINT("ebp      =0x%x\n", ctx->ebp);
    PRINT("esp      =0x%x\n", ctx->esp);
    PRINT("ebx      =0x%x\n", ctx->ebx);
    PRINT("edx      =0x%x\n", ctx->edx);
    PRINT("ecx      =0x%x\n", ctx->ecx);
    PRINT("eax      =0x%x\n", ctx->eax);
    PRINT("eflags   =0x%x\n", ctx->eflags);
    PRINT("eip      =0x%x\n", ctx->eip);
}


//call function entry() with parameter arg
addr_t _os_create_context(addr_t stack_base, size_t stack_size, void (*entry)(void *), void *arg)
{
    // To be filled by students: Project 2
    //int32u_t* sp = (int32u_t*)((int8u_t*)(stack_base) + stack_size/4);
    int32u_t* sp = stack_base + stack_size/4;

    //addr_t sp = (addr_t)((int32u_t)stack_base + stack_size);


    //int32u_t* sp = stack_base + stack_size/4;
    PRINT("stack_base: 0x%x, stack_end: 0x%x\n", stack_base, sp);

    sp-=1;
    *sp = (int32u_t)arg;//save arg
    sp-=1;
    *sp = (int32u_t)NULL;//save return address NULL
    sp-=1;
    *sp = (int32u_t)entry;//save entry
    sp-=1;
    *sp = (int32u_t)1;//save eflags

    //sp-=8*1;
    //save 8 general purpose registers
    for(int i = 0; i < 8; i++){
        sp-=1;
        *sp = (int32u_t)0;
        //sp--;
    }

    _os_context_t* new_cxt = (_os_context_t*)sp;
    //print_context(new_cxt);
    //sp--;
    //*sp = NULL;//4(ebp)
    //sp--;
    //*sp = NULL;//0(ebp)
    PRINT("stack pointer after creating: sp: 0x%x\n", sp);

    return new_cxt;

}


void _os_restore_context(addr_t sp)
{
    //if(sp == NULL){
    //    PRINT("null argument\n");
    //}
    //else{
    //    PRINT("enter restore context\n");
    //}

    addr_t* return_sp; 
    // To be filled by students: Project 2
    __asm__ __volatile__(
        //assembly code
        "movl %0, %%esp         \n\t"//set stack pointer to sp (context to restore)
        "popa                   \n\t"//pop all registers, esp updated
        //"POPF                   \n\t"
        "POPF                  \n\t"
        //"movl 4(%%esp), %%ebp   \n\t"//move ebp up 4
        //"popl %%esp             \n\t"
        //"leave                  \n\t"
        "ret                    \n\t"   //ret back to os_save_context()
        :
        //outputs
        //"=m"(return_sp)
        :
        //inputs
        "m"(sp)
    );
    //PRINT("after restore context\n");
    //print_context(sp);
}

//returns stack pointer after registers saved
addr_t _os_save_context()
{
    // To be filled by students: Project 2
    //begin save context
    __asm__ __volatile__(
        //assemblsy code
        //prologue
        "pushl %%ebp        \n\t"
        "movl %%esp, %%ebp  \n\t"

        //main
        "pushl $resume_eip  \n\t"//push resume addr
        "PUSHF              \n\t"
        "movl $0, %%eax     \n\t"//set eax to 0
        "pusha              \n\t"//push all registers
        "movl %%esp, %%eax  \n\t"//save stack pointer to return to eax
        "pushl 4(%%ebp)     \n\t"//push eip that was at bottom of stack
        "pushl 0(%%ebp)     \n\t"//push ebp
        "movl %%esp, %%ebp  \n\t"//update ebp
        "leave              \n\t"//leave
        "ret                \n\t"//ret back to eos_schedule

        "resume_eip:        \n\t"//give resume point
        "leave              \n\t"
        "ret                \n\t"//ret back to eos_schedule
        : :
    );

}
