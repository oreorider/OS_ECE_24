#include <core/eos.h>
#define STACK_SIZE 8096

static int8u_t stack1[STACK_SIZE]; // stack for task0
static int8u_t stack2[STACK_SIZE]; // stack for task1
static int8u_t stack3[STACK_SIZE]; // stack for task1

static eos_tcb_t tcb1;
static eos_tcb_t tcb2;
static eos_tcb_t tcb3;


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

void task1(){
    while(1){
        PRINT("A\n");
        eos_sleep(0);
    }
}

void task2(){
    while(1){
        PRINT("B\n");
        eos_sleep(0);
    }
}

void task3(){
    while(1){
        PRINT("C\n");
        eos_sleep(0);
    }
}

void eos_user_main()
{
    //PRINT("creating print_numbers task\n");
    //eos_create_task(&tcb0, (addr_t)stack0, STACK_SIZE, print_numbers, NULL, 0);
    //PRINT("creating print_alphabet task\n");
    //eos_create_task(&tcb1, (addr_t)stack1, STACK_SIZE, print_alphabet, NULL, 0);
    eos_create_task(&tcb1, stack1, STACK_SIZE, task1, NULL, 1);
    eos_set_period(&tcb1, 2);

    eos_create_task(&tcb2, stack2, STACK_SIZE, task2, NULL, 10);
    eos_set_period(&tcb2, 4);

    eos_create_task(&tcb3, stack3, STACK_SIZE, task3, NULL, 50);
    eos_set_period(&tcb3, 8);
}
