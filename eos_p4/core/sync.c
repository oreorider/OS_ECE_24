/********************************************************
 * Filename: core/sync.c
 *
 * Author: Jiyong Park, RTOSLab. SNU
 * Modified by: Seongsoo Hong on 03/31/24
 *
 * Description: Routines for semaphores and condition variables
 ********************************************************/

#include <core/eos.h>

void eos_init_semaphore(eos_semaphore_t *sem, int32u_t initial_count, int8u_t queue_type)
{
    // To be filled by students: Project 4
    sem->count = initial_count;
    sem->queue_type = queue_type;
    sem->wait_queue = NULL; //to be determined later
}


int32u_t eos_acquire_semaphore(eos_semaphore_t *sem, int32s_t timeout)
{
    // To be filled by students: Project 4
    hal_disable_interrupt(); //disable interrupt first

    //successful semaphore acquire!
    if(sem->count > 0){
        sem->count -= 1;
        hal_enable_interrupt();
        return 1;
    }

    //at this point, semaphore acquire failed, so enable interrupt again
    hal_enable_interrupt();

    //immedieate return if timeout=-1
    if(timeout == -1){
        return 0;
    }

    //put task to sleep indefinetly
    else if(timeout == 0){
        eos_tcb_t* curr_task = eos_get_current_task();
        curr_task->state = WAITING;

        //if FIFO queue type add to back of queue
        if(sem->queue_type == 0){
            _os_add_node_tail(&(sem->wait_queue), curr_task->node);
        }

        //if priorotiy add depending on prio
        else{
            _os_add_node_priority(&(sem->wait_queue), curr_task->node);
        }

        while(1){

            //call schedule to yield
            eos_schedule();

            //now check if semaphore available again
            hal_disable_interrupt();

            //if semaphore available (successfully got)
            if(sem->count > 0){
                sem->count -= 1;
                _os_remove_node(&(sem->wait_queue), curr_task->node);
                hal_enable_interrupt();
                return 1;
            }

            hal_enable_interrupt();
        }
    }

    //if timeout > 0, sleep for 'timeout' time, then check for semaphore
    else{
        eos_tcb_t* curr_task = eos_get_current_task();
        
        curr_task->state = SLEEPING;

        while(1){
            //add task to wait queue
            if(sem->queue_type == 0){
                _os_add_node_tail(&(sem->wait_queue), curr_task->node);
            }
            else{
                _os_add_node_priority(&(sem->wait_queue), curr_task->node);
            }

            //wake task up after timeout ticks
            eos_counter_t* sys_timer = eos_get_system_timer();
            eos_alarm_t* alarm = (eos_alarm_t*)malloc(sizeof(eos_alarm_t));
            eos_set_alarm(sys_timer, alarm, timeout, _os_wakeup_sleeping_task, curr_task);

            eos_schedule();

            //when task is woke up by another task

            //remove task from waiting queue
            _os_remove_node(&(sem->wait_queue), curr_task->node);

            hal_disable_interrupt();
            //success! semaphore available
            if(sem->count > 0){
                sem->count -= 1;
                hal_enable_interrupt();
                return 1;
            }

            //fail, return 0
            //else if(sem->count <= 0){
            else{
                hal_enable_interrupt();
                return 0;
            }

        }
    }
    return 0;
}


void eos_release_semaphore(eos_semaphore_t *sem)
{
    // To be filled by students: Project 
    //add back semaphore
    hal_disable_interrupt();
    sem->count += 1;
    
    //no tasks to wakeup, just return
    if(sem->wait_queue == NULL){
        hal_enable_interrupt();
        return;
    }

    hal_enable_interrupt();

    //wakeup task in queue
    eos_tcb_t* wake_task = (eos_tcb_t*)(sem->wait_queue->ptr_data);
    _os_remove_node(&(sem->wait_queue), wake_task->node);
    _os_wakeup_sleeping_task(wake_task);
}


/**
 * Condition variables are not covery in the OS course
 */

void eos_init_condition(eos_condition_t *cond, int32u_t queue_type)
{
    cond->wait_queue = NULL;
    cond->queue_type = queue_type;
}


void eos_wait_condition(eos_condition_t *cond, eos_semaphore_t *mutex)
{
    /* Releases acquired semaphore */
    eos_release_semaphore(mutex);
    /* Waits on condition's wait_queue */
    _os_wait(&cond->wait_queue);
    /* Acquires semaphore before returns */
    eos_acquire_semaphore(mutex, 0);
}


void eos_notify_condition(eos_condition_t *cond)
{
    /* Selects a task in wait_queue and wake it up */
    _os_wakeup_single(&cond->wait_queue, cond->queue_type);
}


int8u_t eos_lock_scheduler(){
    int32u_t flag = hal_disable_interrupt();
    int8u_t temp = _os_scheduler_lock;
    _os_scheduler_lock = LOCKED;
    hal_restore_interrupt(flag);
    return temp;
}

void eos_restore_scheduler(int8u_t scheduler_state){
    int32u_t flag = hal_disable_interrupt();
    _os_scheduler_lock = scheduler_state;
    hal_restore_interrupt(flag);
    eos_schedule();
}

int8u_t eos_get_scheduler_lock(){
    return _os_scheduler_lock;
}
