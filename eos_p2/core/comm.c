/********************************************************
 * Filename: core/comm.c
 *
 * Author: Jiyong Park, RTOSLab. SNU
 * Modified by: Seongsoo Hong on 03/30/24
 *
 * Description: Message queue management
 ********************************************************/

#include <core/eos.h>


void eos_init_mqueue(eos_mqueue_t *mq, void *queue_start, int16u_t queue_size, int8u_t msg_size, int8u_t queue_type)
{
    // To be filled by students: Project 4
}


int8u_t eos_send_message(eos_mqueue_t *mq, void *message, int32s_t timeout) 
{
    // To be filled by students: Project 4
}


int8u_t eos_receive_message(eos_mqueue_t *mq, void *message, int32s_t timeout)
{
    // To be filled by students: Project 4
}
