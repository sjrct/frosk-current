//
// include/kernel/paging.h
//

#ifndef _PAGING_H_
#define _PAGING_H_

#include <types.h>

#define KERN_PAGE_FL 3
#define USER_PAGE_FL 7

void setup_paging(void);
void pageto(ulong, ulong);
void page_fault(ulong, ulong);

#endif
