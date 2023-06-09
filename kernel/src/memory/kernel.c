//
// kernel/src/memory/kernel.c
//

#include <util.h>
#include <types.h>
#include <common.h>
#include <stddef.h>
#include "pages.h"
#include "kernel.h"
#include "../debug.h"
#include "../paging.h"
#include "../stubout.h"

#define ADJ_PAGE_SIZE (PAGE_SIZE - (sizeof(chunk_t)))

typedef struct chunk {
    ulong size;
    struct chunk * next;
} chunk_t;

chunk_t * head = NULL;
chunk_t * staged = NULL;

static ulong free_kernel_size = ADJ_PAGE_SIZE;

void setup_kernel_memory(void)
{
    head = (chunk_t *)KMEM_START;
    head->size = ADJ_PAGE_SIZE;
    head->next = head;

    staged = (chunk_t *)(KMEM_START + PAGE_SIZE);
    staged->size = ADJ_PAGE_SIZE;
}

void print_memory(void) {
    int first = 1;
    chunk_t *curr = head;

    do {
        if (first) {
            first = 0;
        } else {
            dprintf("->");
        }

        dprintf("%p(%x)", curr, curr->size);
        curr = head->next;
    } while (curr != head);

    dprintf("\n");
}

void stage_new_page(void) {
    ulong phys, virt;

    // replace the stage page
    virt = alloc_pgs(PAGE_SIZE, VIRT_PAGES);
    phys = alloc_pgs(PAGE_SIZE, PHYS_PAGES);
    assert(!(virt % PAGE_SIZE));
    assert(!(phys % PAGE_SIZE));
    pageto(virt, phys | KERN_PAGE_FL);

    staged = (chunk_t *)virt;
    staged->size = ADJ_PAGE_SIZE;
}

void * _kalloc(ulong size)
{
    ulong adj_size, l;
    ulong phys, virt;
    chunk_t * first;
    chunk_t * prev;
    chunk_t * ret = NULL;

    size = align(size, sizeof(chunk_t));

    if (size > ADJ_PAGE_SIZE) {
        // big alloc, get some new pages
        adj_size = align(size + sizeof(chunk_t), PAGE_SIZE);
        virt = alloc_pgs(adj_size, VIRT_PAGES);
        phys = alloc_pgs(adj_size, PHYS_PAGES);

        assert(!(virt % PAGE_SIZE));
        assert(!(phys % PAGE_SIZE));
        for (l = 0; l < adj_size; l += PAGE_SIZE) {
            pageto(virt + l, (phys + l) | KERN_PAGE_FL);
        }

        first = (chunk_t *)virt;
        first->size = adj_size - sizeof(chunk_t);
        first->next = head->next->next;
        head->next = first;
        head = first;
    } else if (head != NULL) {
        // Look for free data in list
        prev = head;
        head = head->next;
        first = head;

        while (head->size < size) {
            prev = head;
            head = head->next;

            if (head == first) {
                assert(staged != NULL);
                free_kernel_size += staged->size;

                staged->next = head->next;
                head->next = staged;
                head = staged;
                stage_new_page();
                break;
            }
        }
    } else {
        // add the staged to the list
        assert(staged != NULL);
        free_kernel_size += staged->size;

        prev = head = staged;
        head->next = head;
        stage_new_page();
    }

    if (head->size == size) {
        // exact match
        ret = head;

        if (prev == head) {
            head = NULL;
        } else {
            head = head->next;
            prev->next = head;
        }
    } else {
        // larger match
        assert(head->size >= size + sizeof(chunk_t));

        head->size -= size + sizeof(chunk_t);
        ret = (chunk_t *)((char *)(head + 1) + head->size);
        ret->size = size;

        free_kernel_size -= sizeof(chunk_t);
    }

    free_kernel_size -= size;

    return (void *)(ret + 1);
}

void _kfree(void * ptr)
{
    chunk_t * prev, * it;
    chunk_t * ck = (chunk_t *)ptr - 1;

    free_kernel_size += ck->size;

    if (head == NULL) {
        head = ck;
        ck->next = ck;
    } else {
        it = head->next;
        prev = head;

        do {
            if ((ulong)it == (ulong)ptr + ck->size) {
                prev->next = ck;
                ck->next = it->next;
                ck->size += it->size + sizeof(chunk_t);
                free_kernel_size += sizeof(chunk_t);
                return;
            }

            if ((ulong)ck == (ulong)(it + 1) + it->size) {
                it->size += ck->size + sizeof(chunk_t);
                free_kernel_size += sizeof(chunk_t);
                return;
            }

            prev = it;
            it = it->next;
        } while (prev != head);

        ck->next = head->next;
        head->next = ck;
    }
}
