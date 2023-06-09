/*
 * kernel/src/lock.h
 */

#ifndef LOCK_H
#define LOCK_H

typedef unsigned char lock_t;

#define LOCKED   (0)
#define UNLOCKED (1)

// TODO allow to be called multiple times by the same thread
int  trylock(lock_t *);
void getlock(lock_t *);
void unlock (lock_t *);

#define initlock unlock

#endif
