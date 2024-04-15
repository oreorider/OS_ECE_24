#include <core/eos.h>
#define STACK_SIZE 8096

static int8u_t stack0[STACK_SIZE]; // stack for task0
static int8u_t stack1[STACK_SIZE]; // stack for task1

static eos_tcb_t tcb0;
static eos_tcb_t tcb1;


static void print_numbers(void *arg)
{
    int i = 1;
    
    while (1) { 
        //PRINT("enter print numbers\t");
        printf("%d ", i);
        eos_schedule();
        if(i++ == 20){
            i = 1;
        }
    }
    
}

static void print_alphabet(void *arg)
{
    int i = 97;
    while (1) {
        //PRINT("enter print alphabet\t");
        printf("%c ", i);
        eos_schedule();
        if(i++ == 122) {
            i = 97;
        }
    }
}

void eos_user_main()
{
    PRINT("creating print_numbers task\n");
    eos_create_task(&tcb0, (addr_t)stack0, STACK_SIZE, print_numbers, NULL, 0);
    PRINT("creating print_alphabet task\n");
    eos_create_task(&tcb1, (addr_t)stack1, STACK_SIZE, print_alphabet, NULL, 0);
}
