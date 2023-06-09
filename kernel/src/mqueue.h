#ifndef MQUEUE_H
#define MQUEUE_H

typedef struct mqueue mqueue_t;
typedef struct message message_t;

#include "lock.h"

struct mqueue {
    message_t *head, *tail;
    lock_t lock;
};

/* Requires mqueue_t to be complete type */
#include "scheduler.h"

void init_mqueue(mqueue_t *);
void empty_mqueue(mqueue_t *);

void send_msg(void * data, ulong size, mqueue_t * queue);
ulong recv_msg(void * data, ulong size, message_t ** current, mqueue_t * queue);
void consume_msg(message_t ** current, mqueue_t * queue);
void stop_recv(message_t ** current, mqueue_t * queue);

void proc_send_msg(void * data, ulong size, process_t * proc);
ulong proc_recv_msg(void * data, ulong size, message_t ** current);
void proc_consume_msg(message_t ** current);
void proc_stop_recv(message_t ** current);

#endif
