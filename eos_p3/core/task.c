/********************************************************
 * Filename: core/task.c
 *
 * Author: Jiyong Park, RTOSLab. SNU
 * Modified by: Seongsoo Hong on 04/06/24
 *
 * Description: task creation and scheduling
 ********************************************************/

#include <core/eos.h>

#define READY		1
#define RUNNING		2
#define WAITING		3
#define SUSPENDED       4

/**
 * Runqueue of ready tasks
 */
static _os_node_t *_os_ready_queue[LOWEST_PRIORITY + 1];

/**
 * Pointer to TCB of the running task
 */
static eos_tcb_t *_os_current_task;


int32u_t eos_create_task(eos_tcb_t *task, addr_t sblock_start, size_t sblock_size, void (*entry)(void *arg), void *arg, int32u_t priority)
{
    PRINT("task: 0x%x, sblock_start: 0x%x\n", (int32u_t)task, sblock_start);
    //create queueing node
    _os_node_t* queue_node = (_os_node_t*)malloc(sizeof(_os_node_t));
    queue_node->previous = NULL;
    queue_node->next = NULL;
    queue_node->ptr_data = task;
    queue_node->priority = priority;

    //create alarm
    eos_alarm_t* task_alarm = (eos_alarm_t*)malloc(sizeof(eos_alarm_t));
    task_alarm->timeout = 0;
    task_alarm->alarm_queue_node.ptr_data = task;
    task_alarm->alarm_queue_node.previous = NULL;
    task_alarm->alarm_queue_node.next = NULL;
    task_alarm->alarm_queue_node.priority = priority;

    //initialize tcb struct
    task->state = READY;
    task->priority = priority;
    task->period = 0;//unused
    task->context = _os_create_context(
        sblock_start,
        sblock_size,
        entry,
        arg
    );
    task->node = queue_node;
    task->entry = entry;
    task->arg = arg;
    task->alarm = task_alarm;
    


    //add node to ready queue
    _os_add_node_tail(&(_os_ready_queue[priority]), task->node);
    _os_set_ready(priority);

    return 0;
    // To be filled by students: Projects 2 and 3
}


int32u_t eos_destroy_task(eos_tcb_t *task) {
    // To be filled by students: not covered
}

void print_registers(){
    int32u_t* cur_edi;
    int32u_t* cur_esi;
    int32u_t* cur_ebp;
    int32u_t* cur_esp;
    int32u_t* cur_ebx;
    int32u_t* cur_edx;
    int32u_t* cur_ecx;
    int32u_t* cur_eax;
    //int32u_t* cur_eip;
    
    __asm__ __volatile__(
        "movl %%edi, %0     \n\t"
        "movl %%esi, %1     \n\t"
        "movl %%ebp, %2     \n\t"
        "movl %%esp, %3     \n\t"
        "movl %%ebx, %4     \n\t"
        "movl %%edx, %5     \n\t"
        "movl %%ecx, %6     \n\t"
        "movl %%eax, %7     \n\t"
        //"movl %%eip, %8     \n\t"
        :
        "=m"(cur_edi),
        "=m"(cur_esi),
        "=m"(cur_ebp),
        "=m"(cur_esp),
        "=m"(cur_ebx),
        "=m"(cur_edx),
        "=m"(cur_ecx),
        "=m"(cur_eax)
        //"=m"(cur_eip)
    );
    PRINT("cur_edi\t= 0x%x\n", cur_edi);
    PRINT("cur_esi\t= 0x%x\n", cur_esi);
    PRINT("cur_ebp\t= 0x%x\n", cur_ebp);
    PRINT("cur_esp\t= 0x%x\n", cur_esp);
    PRINT("cur_ebx\t= 0x%x\n", cur_ebx);
    PRINT("cur_edx\t= 0x%x\n", cur_edx);
    PRINT("cur_ecx\t= 0x%x\n", cur_ecx);
    PRINT("cur_eax\t= 0x%x\n", cur_eax);
}

void eos_schedule() {
    // To be filled by students: Projects 2 and 3
    
    //task currently running, save context and add back to ready queue
    if(_os_current_task != NULL){
        //save context (save holds stack pointer)
        addr_t save = _os_save_context();

        //save context of current task and set current task to READY add to ready queue
        if(save != NULL){
            _os_current_task->context = save;
            _os_current_task->state = READY;
            _os_add_node_tail(&(_os_ready_queue[_os_current_task->priority]), _os_current_task->node);
            //PRINT("added to ready queue\n")
        }
        else{
            return;
        }
    }
    else{
        //PRINT("no task running\n");
    }

    //restore context of ready task with highest prio
    int32u_t prio = _os_get_highest_priority();
    _os_node_t* h_prio_node = _os_ready_queue[prio];

    //choose next task, set the new task to running and remove from ready queue, restore context to run
    if(h_prio_node != NULL){;
        
        _os_current_task = h_prio_node->ptr_data;//chose next task
        _os_current_task->state = RUNNING;
        _os_remove_node(&(_os_ready_queue[prio]), h_prio_node);//remove from ready queue
        _os_restore_context(_os_current_task->context);
        
    }    
}


eos_tcb_t *eos_get_current_task() {
	return _os_current_task;
}


void eos_change_priority(eos_tcb_t *task, int32u_t priority) {
    // To be filled by students: not covered
}


int32u_t eos_get_priority(eos_tcb_t *task) {
    // To be filled by students: not covered
}


void eos_set_period(eos_tcb_t *task, int32u_t period){
    // To be filled by students: Project 3
    task->period = period;
}


int32u_t eos_get_period(eos_tcb_t *task) {
    // To be filled by students: not covered
}


int32u_t eos_suspend_task(eos_tcb_t *task) {
    // To be filled by students: not covered
}


int32u_t eos_resume_task(eos_tcb_t *task) {
    // To be filled by students: not covered
}


void eos_sleep(int32u_t tick) {
    // To be filled by students: Project 3
    eos_tcb_t* current_task = eos_get_current_task();
    current_task->state = SUSPENDED;

    int32u_t timeout = 0;
    if(tick == 0){
        timeout = eos_get_system_timer()->tick + current_task->period;
    }
    else{
        timeout = eos_get_system_timer()->tick + tick;
    }

    eos_set_alarm(eos_get_system_timer(), current_task->alarm, timeout, _os_wakeup_sleeping_task, current_task);
}


void _os_init_task() {
    PRINT("Initializing task module\n");

    /* Initializes current_task */
    _os_current_task = NULL;

    /* Initializes multi-level ready_queue */
    for (int32u_t i = 0; i < LOWEST_PRIORITY; i++) {
        _os_ready_queue[i] = NULL;
    }
}


void _os_wait(_os_node_t **wait_queue) {
    // To be filled by students: not covered
}


void _os_wakeup_single(_os_node_t **wait_queue, int32u_t queue_type) {
    // To be filled by students: not covered
}


void _os_wakeup_all(_os_node_t **wait_queue, int32u_t queue_type) {
    // To be filled by students: not covered
}


void _os_wakeup_sleeping_task(void *arg) {
    // To be filled by students: Project 3
    eos_tcb_t* wakeup_task = (eos_tcb_t*)arg;
    wakeup_task->state = READY;
    _os_set_ready(wakeup_task->priority);
}
