/*
 * kernel/src/fault.h
 */

#ifndef FAULT_H
#define FAULT_H

#include <types.h>

void setup_faults(void);
void abort(const char *, ulong);

#endif
