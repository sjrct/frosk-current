//
// directive.c
//

#include <stdlib.h>
#include <string.h>
#include "fs.h"
#include "parse.h"
#include "directive.h"

#ifndef countof
#  define countof(X) (sizeof(X) / sizeof(X[0]))
#endif

static void confirm_common(directive_t * d)
{
    if (d->values[2] != NULL)
        d->nd->u.common.perm = parse_perm(d->values[2]);

    if (d->values[3] != NULL)
        d->nd->u.common.owner = get_string(d->values[3]);
}

static int fconfirm(directive_t * d)
{
    if (d->values[0] == NULL) return 0;
    if (d->values[1] == NULL) return 0;

    d->nd = new_file(d->values[0], d->values[1], d->par);
    confirm_common(d);

    return 1;
}

static int dconfirm(directive_t * d)
{
    source_t * src;

    if (d->values[0] == NULL) return 0;

    d->nd = new_dir(d->values[0], d->par);
    confirm_common(d);

    if (d->values[1] != NULL) {
        src = source_string(d->values[1]);
        d->nd = parse_directory(src, d->nd);
        free(src);
    }

    return 1;
}

static int iconfirm(directive_t * d)
{
    if (d->values[0] == NULL || d->values[1] == NULL)
        return 0;

    d->nd = new_indir(d->values[0], d->values[1], d->par);
    confirm_common(d);

    return 1;
}

static int vconfirm(directive_t * d)
{
    if (d->values[0] == NULL) return 0;
    if (d->values[1] == NULL) return 0;
    if (d->values[4] == NULL) return 0;

    d->nd = new_virt(d->values[0], atoi(d->values[1]), atoi(d->values[4]), d->par);
    confirm_common(d);

    return 1;
}

static char * ffields[] = {
    "name",
    "actual",
    "perm",
    "owner",
};

static char * dfields[] = {
    "name",
    "contents",
    "perm",
    "owner",
};

static char * ifields[] = {
    "name",
    "target",
    "perm",
    "owner",
};

static char * vfields[] = {
    "name",
    "offset",
    "perm",
    "owner",
    "size",
};

static directive_class_t classes[] = {
    { 'f', fconfirm, countof(ffields), ffields },
    { 'd', dconfirm, countof(dfields), dfields },
    { 'i', iconfirm, countof(ifields), ifields },
    { 'v', vconfirm, countof(vfields), vfields },
};

directive_t * get_directive(int mark, node_t * par)
{
    uint i;
    int j;
    directive_t * d;

    for (i = 0; i < countof(classes); i++) {
        if (classes[i].mark == mark) {
            d = malloc(sizeof(directive_t));

            d->cls = &classes[i];
            d->par = par;
            d->values = malloc(classes[i].field_count * sizeof(char *));
            d->nd = NULL;

            for (j = 0; j < classes[i].field_count; j++)
                d->values[j] = NULL;

            return d;
        }
    }

    return NULL;
}

int confirm_directive(directive_t * d)
{
    int i;
    int r = d->cls->confirm(d);

    for (i = 0; i < d->cls->field_count; i++)
        free(d->values[i]);

    return r;
}

int set_field(directive_t * d, const char * what, char * to)
{
    int i;

    for (i = 0; i < d->cls->field_count; i++) {
        if (!strcmp(what, d->cls->fields[i])) {
            if (d->values[i] != NULL) {
                free(d->values[i]);
            }

            d->values[i] = to;
            return 1;
        }
    }

    return 0;
}
