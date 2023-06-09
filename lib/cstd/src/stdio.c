#include <frosk.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <math.h>

FILE _stdin = {
    .buffer = NULL,
    .buffer_size = 0,
    .buffer_length = 0,
};

// TODO ferror

FILE *fopen(const char *fn, const char *_attr) {
    FILE *f;
    file_t inter_f;

    /* TODO from cd if not starting with / */
    inter_f = fs_acquire(fn, 0);
    if (!inter_f) return NULL;

    f = calloc(1, sizeof(FILE));
    f->file = inter_f;

    return f;
}

int fclose(FILE *f) {
    fs_release(f->file);
    free(f->buffer);
    free(f);
    return 0;
}

size_t fread(void * buf, size_t n, size_t sz, FILE *f) {
    if (f == stdin || f == stdout || f == stderr) {
        // TODO
        return 0;
    }

    fs_read(buf, f->cursor, n * sz, f->file);
    fseek(f, n * sz, SEEK_CUR);

    return n * sz;
}

size_t fwrite(const void * buf, size_t n, size_t sz, FILE * f) {
    if (f == stdin || f == stdout || f == stderr) {
        // TODO
        return 0;
    }

    fs_write(buf, f->cursor, n * sz, f->file);
    fseek(f, n * sz, SEEK_CUR);

    return n * sz;
}

int fprintf(FILE *f, const char *fmt, ...) {
    va_list ls;
    size_t sz;
    char *buf;

    va_start(ls, fmt);
    sz = vsnprintf(NULL, 0, fmt, ls);
    va_end(ls);

    buf = malloc(sz + 1);
    va_start(ls, fmt);
    vsprintf(buf, fmt, ls);
    va_end(ls);

    fputs(buf, f);
    free(buf);

    return sz;
}

int sprintf(char *buf, const char *fmt, ...) {
    int ret;
    va_list ls;
    va_start(ls, fmt);
    ret = vsprintf(buf, fmt, ls);
    va_end(ls);
    return ret;
}

int snprintf(char *buf, size_t len, const char *fmt, ...) {
    int ret;
    va_list ls;
    va_start(ls, fmt);
    ret = vsnprintf(buf, len, fmt, ls);
    va_end(ls);
    return ret;
}

/* TODO move to stdlib? */
char * utoa(ulong val, char *buf, int radix, int letter_a) {
    char ch;
    int i;
    ulong div, tmp;

    if (val == 0) {
        strcpy(buf, "0");
        return buf;
    }

    div = 1;
    tmp = val / radix;

    while (tmp > 0) {
        tmp /= radix;
        div *= radix;
    }

    i = 0;
    while (div > 0) {
        ch = val / div;
        val %= div;
        div /= radix;

        if (ch < 10) ch += '0';
        else ch += letter_a - 10;

        buf[i++] = ch;
    }
    buf[i] = '\0';

    return buf;
}

char * itoa(long v, char *buf, int radix, int letter_a) {
    if (v < 0) {
        buf[0] = '-';
        utoa(-v, buf + 1, radix, letter_a);
    } else {
        utoa(v, buf, radix, letter_a);
    }
    return buf;
}

char * ftoa(double v, char *buf0) {
    int i = 0;
    int max_post = 10;
    int radix = 10;
    char * buf = buf0;
    double pre, post;

    pre = floor(v);
    post = v - pre;

    itoa((long)pre, buf, radix, 'a');
    buf += strlen(buf);

    while (floor(post) != post && i < max_post) {
        post *= radix;
    }

    if (post != 0) {
        buf[0] = '.';
        utoa((long)post, buf + 1, radix, 'a');
    }

    return buf0;
}

int vsprintf(char *buf, const char *fmt, va_list ls) {
    return vsnprintf(buf, INT_MAX, fmt, ls);
}

enum SIZE_CLASS {
    UNDEF,
    HALF,
    HALF_HALF,
    LONG,
    LONG_LONG,
    LONG_DBL,
    MAX,
    SIZE,
    PTRDIFF,
};

static enum SIZE_CLASS get_size_class(const char *fmt, size_t *j) {
    // j -> %
    ++*j;
    // j -> first format char
    switch (fmt[(*j)++]) {
    // j -> second format char
    case 'h':
        if (fmt[*j] == 'h') {
            ++*j;
            return HALF_HALF;
        } else {
            return HALF;
        }
        break;
    case 'l':
        if (fmt[*j] == 'l') {
            ++*j;
            return LONG_LONG;
        } else {
            return LONG;
        }
        break;
    case 'j':
        return MAX;
    case 'z':
        return SIZE;
    case 't':
        return PTRDIFF;
    case 'L':
        return LONG_DBL;
    default:
        // Not a size class specifier
        --*j;
        return UNDEF;
    }
}

int format_int( char *buf
              , size_t i
              , size_t len
              , va_list ls
              , enum SIZE_CLASS size_class
              , int radix
              , int letter_a
) {
    char numbuf[128];
    long long val;

    switch (size_class) {
    case LONG:
        val = va_arg(ls, long);
        break;
    case LONG_LONG:
        val = va_arg(ls, long long);
        break;
    case MAX:
        val = va_arg(ls, intmax_t);
        break;
    case SIZE:
        val = va_arg(ls, size_t);
        break;
    case PTRDIFF:
        val = va_arg(ls, ptrdiff_t);
        break;
    /* promoted to unsigned when passed as params */
    case HALF_HALF:
    case HALF:
    default:
        val = va_arg(ls, int);
        break;
    }

    itoa(val, numbuf, 10, letter_a);
    if (i < len) strncpy(buf + i, numbuf, len - i);
    return strlen(numbuf);
}

int format_uint( char *buf
              , size_t i
              , size_t len
              , va_list ls
              , enum SIZE_CLASS size_class
              , int radix
              , int letter_a
) {
    char numbuf[128];
    unsigned long long val;

    switch (size_class) {
    case LONG:
        val = va_arg(ls, unsigned long);
        break;
    case LONG_LONG:
        val = va_arg(ls, unsigned long long);
        break;
    case MAX:
        val = va_arg(ls, uintmax_t);
        break;
    case SIZE:
        val = va_arg(ls, size_t);
        break;
    case PTRDIFF:
        val = va_arg(ls, ptrdiff_t);
        break;
    /* promoted to unsigned when passed as params */
    case HALF_HALF:
    case HALF:
    default:
        val = va_arg(ls, unsigned);
        break;
    }

    utoa(val, numbuf, 10, letter_a);
    if (i < len) strncpy(buf + i, numbuf, len - i);
    return strlen(numbuf);
}

int vsnprintf(char *buf, size_t len, const char *fmt, va_list ls) {

    size_t i, j;
    double dbl;
    char *s;
    char numbuf[100];
    void *ptr;

    enum SIZE_CLASS size_class;

    for (i = j = 0; fmt[j] != 0; i++, j++) {
        if (fmt[j] == '%') {
            size_class = get_size_class(fmt, &j);

            switch (fmt[j]) {
            case 'c':
                // TODO wint_t
                if (i < len) buf[i] = va_arg(ls, int);
                break;
            case 's':
                // TODO wchar_t
                s = va_arg(ls, char *);
                if (i < len) strncpy(buf + i, s, len - i);
                i += strlen(s) - 1;
                break;
            case 'd':
            case 'i':
                i += format_int(buf, i, len, ls, size_class, 10, 'a') - 1;
                break;
            case 'u':
                i += format_uint(buf, i, len, ls, size_class, 10, 'a') - 1;
                break;
            case 'x':
                i += format_uint(buf, i, len, ls, size_class, 0x10, 'a') - 1;
                break;
            case 'X':
                i += format_uint(buf, i, len, ls, size_class, 0x10, 'A') - 1;
                break;
            case 'o':
                i += format_uint(buf, i, len, ls, size_class, 010, 'a') - 1;
                break;
            case 'g':
                dbl = va_arg(ls, double);
                ftoa(dbl, numbuf);
                if (i < len) strncpy(buf + i, numbuf, len - i);
                i += strlen(numbuf) - 1;
                break;
            case 'p':
                ptr = va_arg(ls, void *);
                if (ptr == NULL) {
                    if (i < len) strncpy(buf + i, "(null)", len - i);
                    i += sizeof("(null)") - 2;
                } else {
                    utoa((ulong)ptr, numbuf, 0x10, 'A');
                    if (i < len) strncpy(buf + i, numbuf, len - i);
                    i += strlen(numbuf) - 1;
                }
                break;
            case '%':
                if (i < len) buf[i] = '%';
                break;
            default:
                if (i < len) buf[i] = '%';
                j--;
            }
        } else {
            if (i < len) buf[i] = fmt[j];
        }
    }

    if (i == len) {
        if (i > 0) buf[i - 1] = 0;
    } if (i < len) {
        buf[i] = 0;
    } else {
        if (len > 0) buf[len - 1] = 0;
    }

    return i;
}

int fputc(int ch, FILE *f) {
    if (f == stdin) return EOF;

    if (f == stdout || f == stderr) {
        // TODO stderr
        proc_send_msg(&ch, sizeof(char), parent_pid);
    } else {
        // TODO test
        fs_write((byte *)&ch, f->cursor, sizeof(char), f->file);
        f->cursor += sizeof(char);
    }

    return ch;
}

int fputs(const char *buf, FILE *f) {
    size_t len;

    if (f == stdin) return EOF;

    len = strlen(buf);

    if (f == stdout || f == stderr) {
        // TODO stderr
        proc_send_msg(buf, len, parent_pid);
    } else {
        // TODO test
        fs_write((const byte *)buf, f->cursor, len, f->file);
        fseek(f, len, SEEK_CUR);
    }

    return len;
}

int puts(const char *str) {
    return printf("%s\n", str);
}

static void unbuffer(size_t sz, FILE *f) {
    size_t i;

    // TODO memmove?
    for (i = 0; i < sz && i + sz < f->buffer_length; i++) {
        f->buffer[i] = f->buffer[i + sz];
    }

    f->buffer_length -= sz;
}

char * fgets(char * buf0, size_t max, FILE * f) {
    char *buf = buf0;
    size_t i, available, sz;
    next_message_t next = 0;

    max -= 1; // for null terminator

    if (f == stdout || f == stderr) {
        return NULL;
    }

    if (f == stdin) {

        while (1) {
            // Fill the buffer from the message queue
            do {
                available = f->buffer_size - f->buffer_length;
                sz = proc_recv_msg(f->buffer, available, &next);

                if (sz > available) {
                    // TODO better resizing?
                    f->buffer_size += sz - available;
                    f->buffer = realloc(f->buffer, f->buffer_size);
                    available = sz;
                    sz = proc_recv_msg(f->buffer, available, &next);
                }

                f->buffer_length += sz;
                proc_consume_msg(&next);
            } while (next != 0);
            proc_stop_recv(&next);

            // Get characters from the buffer
            if (f->buffer_length > 0) {
                for (i = 0; i < f->buffer_length; i++) {
                    *buf = f->buffer[i];
                    if (*buf == '\n' || max == 0) {
                        // Done
                        buf[1] = 0;
                        free(f->buffer);
                        f->buffer = NULL;
                        f->buffer_size = f->buffer_length = 0;
                        //unbuffer(i + 1, f);
                        return buf0;
                    } else if (*buf == '\b') {
                        if (buf > buf0) {
                            buf--;
                            max++;
                        }
                    } else {
                        max--;
                        buf++;
                    }
                }
                unbuffer(i, f);
            }
        }
    } else {
        // TODO
        return NULL;
    }
}

/*
char * fgets(char * buf0, size_t max, FILE * f) {
    size_t sz, i;
    char * ibuf;
    char * buf = buf0;
    next_message_t next = 0;

    max -= 1; // for null terminator

    if (f == stdout || f == stderr) {
        return NULL;
    }

    if (f == stdin) {
        // Look at stuff already in the input buffer
        if (f->buffer_length) {
            // TODO reduce code dupe?
            for (i = 0; i < f->buffer_length; i++) {
                if (f->buffer[i] == '\n' || i == max) {
                    if (i != max) i += 1;
                    unbuffer(i, f);
                    buf[i] = 0;

                    return buf0;
                }
            }

            unbuffer(i, f);
            buf += i;
            max -= i;
        }

        while (max) {
            sz = proc_recv_msg(buf, max, &next);
            if (sz > max) {
                ibuf = malloc(sz);
                sz = proc_recv_msg(ibuf, sz, &next);
                proc_consume_msg(&next);
                proc_stop_recv(&next);
                memcpy(buf, ibuf, max);
            } else {
                ibuf = buf;
            }

            // Look for null terminator
            for (i = 0; i < sz; i++) {
                if (ibuf[i] == '\n' || i == max) {
                    if (i != max) i += 1;
                    buffer(ibuf + i, sz - i, f);
                    buf[i] = 0;

                    if (sz > max) free(ibuf);
                    return buf0;
                }
            }

            buf += i;
            max -= i;
        }

        *buf = 0;
    } else {
        // TODO
        return NULL;
    }

    return buf0;
}
*/

int fseek(FILE * f, long offset, enum _seek seek) {
    if (f == stdin || f == stdout || f == stderr) {
        return 1;
    }

    ulong sz = fs_size(f->file);

    switch (seek) {
    case SEEK_CUR:
        offset += f->cursor;
        break;
    case SEEK_END:
        offset += sz;
        break;
    case SEEK_SET:
        break;
    }

    if (offset < 0) offset = 0;
    else if ((ulong)offset > sz) offset = sz;

    f->cursor = offset;
    return 0;
}

long ftell(FILE *f) {
    if (f == stdin || f == stdout || f == stderr) {
        return 0;
    }
    return f->cursor;
}

int feof(FILE *f) {
    if (f == stdout || f == stderr || f == stdin) {
        // TODO
        return 0;
    }
    // FIXME use actual indicator
    return fs_size(f->file) <= f->cursor;
}
