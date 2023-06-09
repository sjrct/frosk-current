#include <stddef.h>
#include <string.h>
#include "mqueue.h"
#include "debug.h"
#include "memory/kernel.h"
#include "stubout.h"

struct message {
    void *data;
    ulong size;
    message_t *next, *prev;
};

void init_mqueue(mqueue_t *q) {
    q->head = q->tail = NULL;
    initlock(&q->lock);
}

static message_t *create_message(void *data, ulong size) {
    message_t *m = kalloc(sizeof *m);
    m->next = m->prev = NULL;
    m->data = kalloc(size);
    m->size = size;
    memcpy(m->data, data, size);
    return m;
}

static void lock_queue(mqueue_t * queue) {
    //dprintf("locking  %p on %p\n", queue, cur_thrd);
    getlock(&queue->lock);
    //dprintf("locked   %p on %p\n", queue, cur_thrd);
}

static void unlock_queue(mqueue_t * queue) {
    //dprintf("unlocked %p on %p\n", queue, cur_thrd);
    unlock(&queue->lock);
}

static void destroy_message(message_t * m) {
    kfree(m->data);
    kfree(m);
}

static int is_empty(mqueue_t *q) {
    return q->head == NULL;
}

void empty_mqueue(mqueue_t * q) {
    message_t *m;
    lock_queue(q);
    for (m = q->head; m != NULL; m = m->next) {
        destroy_message(m);
    }
    q->head = q->tail = NULL;
    unlock_queue(q);
}

void send_msg(void * data, ulong size, mqueue_t * queue) {

    message_t *m;

    lock_queue(queue);
    m = create_message(data, size);

    if (is_empty(queue)) {
        /* Only message in queue */
        queue->head = queue->tail = m;
    } else {
        /* Add to head of queue */
        queue->head->prev = m;
        m->next = queue->head;
        queue->head = m;
    }

    unlock_queue(queue);
}

/**
 * Put the message data into the given data buffer. Unless the message doesn't fit
 * into the buffer (of the given size), in which case it is left untouched and
 * the necessary size is returned. If there is nothing in the queue, 0 is returned
 * otherwise the size of the message is returned.
 *
 * *current should be NULL when first starting to iterate over the queue. This function
 * will update it to the next message in the queue when called again.
 */
ulong recv_msg(void * data, ulong size, message_t ** current, mqueue_t * queue) {
    if (*current == NULL) {
        lock_queue(queue);
        *current = queue->tail;
    } else {
        *current = (*current)->prev;
    }

    if (*current == NULL) {
        unlock_queue(queue);
        return 0;
    }

    if (size >= (*current)->size) {
        memcpy(data, (*current)->data, (*current)->size);
    }

    return (*current)->size;
}

/**
 * Call this is your program is to consume a message received with recv_msg, to
 * remove it from the queue. Not calling this means that you are skipping the
 * message, ie. leaving it be consumed elsewhere.
 */
void consume_msg(message_t ** current, mqueue_t * queue) {
    message_t *m = *current;

    if (*current) {
        /* Remove it from the list */
        if (m == queue->tail) {
            queue->tail = m->prev;
            if (m->prev) m->prev->next = NULL;
            if (m == queue->head) queue->head = NULL;
            *current = NULL;
        } else if (m == queue->head) {
            assert(m->next != NULL);
            queue->head = m->next;
            m->next->prev = NULL;
        } else {
            if (m->prev) m->prev->next = m->next;
            if (m->next) m->next->prev = m->prev;
            *current = m->next;
        }
    }

    if (*current != NULL) {
        /* Free the data */
        kfree(m->data);
        kfree(m);
    } else {
        /* Undo the lock if we've stopped iterating */
        unlock_queue(queue);
    }
}

void stop_recv(message_t ** current, mqueue_t * queue) {
    if (*current != NULL) {
        unlock_queue(queue);
    }
}

process_t *resolve_proc(process_t *proc) {
    if (proc == NULL) {
        /* 0 signals parenet process */
        return cur_thrd->parent->parent;
    } else {
        return proc;
    }
}

void proc_send_msg(void * data, ulong size, process_t * proc) {
    proc = resolve_proc(proc);
    if (proc != NULL)
        send_msg(data, size, &proc->mqueue);
}

ulong proc_recv_msg(void * data, ulong size, message_t ** current) {
    process_t *proc = cur_thrd->parent;
    assert(proc != NULL);
    return recv_msg(data, size, current, &proc->mqueue);
}

void proc_consume_msg(message_t ** current) {
    process_t *proc = cur_thrd->parent;
    assert(proc != NULL);
    consume_msg(current, &proc->mqueue);
}

void proc_stop_recv(message_t ** current) {
    process_t *proc = cur_thrd->parent;
    assert(proc != NULL);
    stop_recv(current, &proc->mqueue);
}
