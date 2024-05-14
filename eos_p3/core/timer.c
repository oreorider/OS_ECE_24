/********************************************************
 * Filename: core/timer.c
 *
 * Author: Jiyong Park, RTOSLab. SNU
 * Modified by: Seongsoo Hong on 03/30/24
 *
 * Description: User interface for handling interrupts
 ********************************************************/

#include <core/eos.h>

static eos_counter_t system_timer;


int8u_t eos_init_counter(eos_counter_t *counter, int32u_t init_value)
{
    counter->tick = init_value;
    counter->alarm_queue = NULL;

    return 0;
}


void eos_set_alarm(eos_counter_t* counter, eos_alarm_t* alarm, int32u_t timeout, void (*entry)(void *arg), void *arg)
{
    // To be filled by students: Project 3
    //remove alarm from alarm queue
    _os_remove_node(&(counter->alarm_queue), &(alarm->alarm_queue_node));

    //return if 0 or entry is null
    if(timeout == 0 || entry == NULL){
        return;
    }

    //fil up alarm struct
    alarm->timeout = timeout;
    alarm->handler = entry;
    alarm->arg = arg;
    alarm->alarm_queue_node.ptr_data = alarm;
    alarm->alarm_queue_node.priority = timeout;

    //add alarm to counter's alarm queue
    _os_add_node_priority(&(counter->alarm_queue), &(alarm->alarm_queue_node));
}


eos_counter_t* eos_get_system_timer()
{
    return &system_timer;
}


void eos_trigger_counter(eos_counter_t* counter)
{
    PRINT("tick\n");
    // To be filled by students: Project 3
    //increase counter tick by 1
    counter->tick+=1;

    //check counter's alarm queue and call callback function if finished
    while(counter->alarm_queue != NULL){

        //if next alarm is not equal to current counter tick, do nothing
        eos_alarm_t* alarm_next = counter->alarm_queue->ptr_data;
        if(alarm_next->timeout != counter->tick){
            break;
        }
        //if next alarm is equal to current counter tick, set alarm that activates
        else{
            eos_set_alarm(counter, alarm_next, 0, NULL, NULL);//will remove alarm_next from counter queue
            alarm_next->handler(alarm_next->arg);
        }
    }

    eos_schedule();
    
}


/* Timer interrupt handler */
static void timer_interrupt_handler(int8s_t irqnum, void *arg)
{
    /* Triggers alarms */
    eos_trigger_counter(&system_timer);
}


void _os_init_timer()
{
    eos_init_counter(&system_timer, 0);

    /* Registers timer interrupt handler */
    eos_set_interrupt_handler(IRQ_INTERVAL_TIMER0,
		    timer_interrupt_handler, NULL);
}
