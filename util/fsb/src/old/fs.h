//
// util/fsb/src/fs.h
//

#ifndef FS_H
#define FS_H

#include <stdio.h>

#define PERM_READ_OWN 0x01
#define PERM_WRIT_OWN 0x04
#define PERM_EXEC_OWN 0x10
#define PERM_READ_ALL 0x03
#define PERM_WRIT_ALL 0x0C
#define PERM_EXEC_ALL 0x30

#define FLAG_VIRT 1

typedef struct node {
    unsigned char perm;
    unsigned char flags;

    enum {
        TYPE_FILE,
        TYPE_DIR,
        TYPE_IND,
        TYPE_BLOCK,
        TYPE_STR,
    } type;

    union {
        struct {
            node_t * name;
            node_t * data;
            node_t * owner;
            node_t * next;
        } cmn;

        struct {
            node_t * 
        }
    } u;
} node_t;

typedef struct fs {
    node_t * root;
} fs_t;

fs_t * create_fs(unsigned, char *, unsigned);
void write_fs(fs_t *, FILE *, unsigned);

#endif
