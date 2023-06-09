#include <stdlib.h>
#include <string.h>

// FIXME? better size? dupped define
// FIXME? 32-bit mode?
#define LOW_HALF_TOP  0x800000000000
#define HALF_LOW      (LOW_HALF_TOP / 2)
#define HEAP_SIZE     (HALF_LOW - sizeof(chunk_t) - (size_t)&_heap_bottom)

// FIXME dupped define
#define align(X, TO) ((X) % (TO) ? (X) - ((X) % (TO)) + (TO) : (X))

/* Keep this sorted lowest->highest to prioritize reusing lower addresses so
 * that we don't keep allocating more pages. */
typedef struct chunk {
    struct chunk *next;
    size_t size;
} chunk_t;

#define CHUNK_TOP(C) ((void *)((char *)((C) + 1) + (C)->size))

extern chunk_t _heap_bottom;
static chunk_t *head = NULL;

void *malloc(size_t size) {
    chunk_t *ch;
    chunk_t **pnext;

    size = align(size, sizeof(chunk_t));

    if (head == NULL) {
        /* First malloc call */
        head = (chunk_t *)align((size_t)&_heap_bottom, sizeof(chunk_t));
        head->size = HEAP_SIZE;
        head->next = NULL;
    }

    pnext = &head;
    for (ch = head; ch != NULL; ch = ch->next) {
        if (size <= ch->size) {
            if (ch->size <= size + sizeof(chunk_t)) {
                /* Exact match, or broken off chunk would be of size 0 */
                *pnext = ch->next;
            } else {
                /* Divide the chunk */
                *pnext = ch + 1 + size / sizeof(chunk_t);
                (*pnext)->next = ch->next;
                (*pnext)->size = ch->size - size - sizeof(chunk_t);
            }
            return (void *)(ch + 1);
        }

        pnext = &ch->next;
    }

    return NULL;
}

void free(void *p) {
    chunk_t *prior;
    chunk_t *chunk = (chunk_t *)p - 1;

    if (p == NULL) return;

    /* TODO head == NULL? */

    if (chunk < head) {
        /* Chunk is earliest */
        if (CHUNK_TOP(chunk) == head) {
            chunk->size += head->size + sizeof(chunk);
            chunk->next = head->next;
        } else {
            chunk->next = head;
        }
        head = chunk;
    } else {
        /* Find the chunk prior to the freed one */
        /* The list is sorted from lowest to highest by address! */
        prior = head;
        while (prior->next != NULL && prior->next < chunk) {
            prior = prior->next;
        }

        if (CHUNK_TOP(prior) == chunk) {
            /* Combine chunk into prior */
            prior->size += sizeof(chunk) + chunk->size;

            /* Is the chunk a perfect hole between prior and prior->next? */
            if (CHUNK_TOP(prior) == prior->next) {
                prior->size += prior->next->size + sizeof(chunk_t);
                prior->next = prior->next->next;
            }
        } else if (CHUNK_TOP(chunk) == prior->next) {
            /* Combine prior->next into chunk */
            chunk->next = prior->next->next;
            chunk->size += prior->next->size + sizeof(chunk);
            prior->next = chunk;
        } else {
            chunk->next = prior->next;
            prior->next = chunk;
        }
    }
}

void *calloc(size_t count, size_t size) {
    void *p = malloc(count * size);
    memset(p, 0, count * size);
    return p;
}

void *realloc(void *old, size_t size) {
    void *new;
    size_t copy_size;
    chunk_t *chunk;

    if (old) {
        chunk = (chunk_t *)old - 1;

        copy_size = chunk->size < size ? chunk->size : size;

        new = malloc(size);
        memcpy(new, old, copy_size);
        free(old);
    } else {
        new = malloc(size);
    }

    return new;
}
