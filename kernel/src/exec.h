/*
 * exec.h
 */

#ifndef EXEC_H
#define EXEC_H

#include "fs/fs.h"
#include "scheduler.h"

process_t * bexec(const byte *, ulong, int, const char **);
process_t * fexec(const char *, int, const char **, fs_entry_t *);

#endif
