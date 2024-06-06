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
    eos_semaphore_t* putsem = (eos_semaphore_t*)malloc(sizeof(eos_semaphore_t));
    eos_semaphore_t* getsem = (eos_semaphore_t*)malloc(sizeof(eos_semaphore_t));
    eos_init_semaphore(putsem, (int32u_t)queue_size, queue_type);
    eos_init_semaphore(getsem, 0, queue_type);

    mq->queue_size = queue_size;
    mq->msg_size = msg_size;
    mq->queue_start = queue_start;
    mq->queue_end = (int8u_t*)(mq->queue_start) + queue_size;
    mq->front = queue_start;
    mq->rear = (int8u_t*)queue_start-1;
    mq->queue_type = queue_type;
    mq->putsem = putsem;
    mq->getsem = getsem;
}


int8u_t eos_send_message(eos_mqueue_t *mq, void *message, int32s_t timeout) 
{
    // To be filled by students: Project 4
    int32u_t putsem_successful = eos_acquire_semaphore(mq->putsem, timeout);

    //if successfully got semaphore
    if(putsem_successful){
        for(int32u_t i = 0;i < (mq->msg_size); i++){
            mq->rear++;
            //cast mq->rear into 8bit int, then de-reference to get char
            //then cast message into 8bit int, then access ith element
            *((int8u_t*)mq->rear) = ((int8u_t*)message)[i];

            //if reached end of queue memory region, wrap around
            if(mq->queue_end <= mq->rear){
                mq->rear = (int8u_t*)(mq->queue_start) - 1;
            }

            //release semaphore after adding to queue
            eos_release_semaphore(mq->getsem);
        }
    }
    return 0;
}


int8u_t eos_receive_message(eos_mqueue_t *mq, void *message, int32s_t timeout)
{
    // To be filled by students: Project 4
    int32u_t getsum_successful = eos_acquire_semaphore(mq->getsem, timeout);

    if(getsum_successful){
        for(int32u_t i = 0;i < (mq->msg_size); i++){
            ((int8u_t*)message)[i] = *((int8u_t*)(mq->front));
            mq->front++;

            //if front is after end memory location, reset it
            if(mq->queue_end < mq->front){
                mq->front = mq->queue_start;
            }

            eos_release_semaphore(mq->putsem);
        }
    }

    return 0;
}
