#include <frosk.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include "fcurses.h"

#define ROUGH_CLOCK_SEC ((qword)0x20000000)

#define DBOX_VERT 0xBA
#define DBOX_HORZ 0xCD
#define DBOX_TL_C 0xC9
#define DBOX_TR_C 0xBB
#define DBOX_BL_C 0xC8
#define DBOX_BR_C 0xBC

typedef struct key_state {
    int lshift, rshift;
} key_state_t;

#define RELEASE 0x80
#define LSHIFT 0x2A
#define RSHIFT 0x36

extern int _heap_bottom;

static char scan_code1_to_char[] = {
    0,
    0, // esc
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    '0',
    '-',
    '=',
    '\b', // backspace
    '\t',
    'q',
    'w',
    'e',
    'r',
    't',
    'y',
    'u',
    'i',
    'o',
    'p',
    '[',
    ']',
    '\n', // enter
    0, // left ctrl
    'a',
    's',
    'd',
    'f',
    'g',
    'h',
    'j',
    'k',
    'l',
    ';',
    '\'',
    '`',
    0, // left shift
    '\\',
    'z',
    'x',
    'c',
    'v',
    'b',
    'n',
    'm',
    ',',
    '.',
    '/',
    0, // right shift
    '*', // keypad
    0, // left alt
    ' ',
    0, // Caps lock
    0, // F1
    0, // F2
    0, // F3
    0, // F4
    0, // F5
    0, // F6
    0, // F7
    0, // F8
    0, // F9
    0, // F10
    0, // num lock
    0, // scroll lock
    '7', // keypad
    '8', // keypad
    '9', // keypad
    '-', // keypad
    '4', // keypad
    '5', // keypad
    '6', // keypad
    '+', // keypad
    '1', // keypad
    '2', // keypad
    '3', // keypad
    '0', // keypad
    '.', // keypad
    0, // undef,
    0, // undef
    0, // f11
    0, // f12
};

static char scan_code1_to_char_shifted[] = {
    0,
    0, // esc
    '!',
    '@',
    '#',
    '$',
    '%',
    '^',
    '&',
    '*',
    '(',
    ')',
    '_',
    '+',
    '\b', // backspace
    '\t',
    'Q',
    'W',
    'E',
    'R',
    'T',
    'Y',
    'U',
    'I',
    'O',
    'P',
    '{',
    '}',
    '\n', // enter
    0, // left ctrl
    'A',
    'S',
    'D',
    'F',
    'G',
    'H',
    'J',
    'K',
    'L',
    ':',
    '"',
    '~',
    0, // left shift
    '?',
    'Z',
    'X',
    'C',
    'V',
    'B',
    'N',
    'M',
    '<',
    '>',
    '?',
    0, // right shift
    '*', // keypad
    0, // left alt
    ' ',
    0, // Caps lock
    0, // F1
    0, // F2
    0, // F3
    0, // F4
    0, // F5
    0, // F6
    0, // F7
    0, // F8
    0, // F9
    0, // F10
    0, // num lock
    0, // scroll lock
    '7', // keypad
    '8', // keypad
    '9', // keypad
    '-', // keypad
    '4', // keypad
    '5', // keypad
    '6', // keypad
    '+', // keypad
    '1', // keypad
    '2', // keypad
    '3', // keypad
    '0', // keypad
    '.', // keypad
    0, // undef,
    0, // undef
    0, // f11
    0, // f12
};


static void halt(void) {
    for (;;);
}

void wait_clocks(qword length) {
    qword start = rdtsc();
    while (rdtsc() - start < length);
}

void box_screen(fc_screen_t *scr, int style) {
    int i;

    fc_putc(scr, DBOX_TL_C, 0, 0, style);
    fc_putc(scr, DBOX_BL_C, 0, scr->height - 1, style);
    fc_putc(scr, DBOX_TR_C, scr->width - 1, 0, style);
    fc_putc(scr, DBOX_BR_C, scr->width - 1, scr->height - 1, style);

    for (i = 1; i < scr->height - 1; i++) {
        fc_putc(scr, DBOX_VERT, 0, i, style);
        fc_putc(scr, DBOX_VERT, scr->width - 1, i, style);
    }

    for (i = 1; i < scr->width - 1; i++) {
        fc_putc(scr, DBOX_HORZ, i, 0, style);
        fc_putc(scr, DBOX_HORZ, i, scr->height - 1, style);
    }
}

void process_key(byte scancode, fc_window_t *win, key_state_t *state, pid_t pid) {
    char ch;

    if (scancode >= RELEASE) {
        if (scancode == (LSHIFT | RELEASE)) state->lshift = 0;
        if (scancode == (RSHIFT | RELEASE)) state->rshift = 0;
    } else {
        if (scancode == LSHIFT) state->lshift = 1;
        if (scancode == RSHIFT) state->rshift = 1;

        if (scancode < sizeof(scan_code1_to_char)) {
            ch = (state->lshift || state->rshift)
                ? scan_code1_to_char_shifted[scancode]
                : scan_code1_to_char[scancode];
            if (ch != 0) {
                fc_wputc(win, ch, 14);
                proc_send_msg(&ch, sizeof(ch), pid);
            }
        }
    }
}

/*
void check_stack(fc_screen_t *scr) {
    char label[] = "*buf=";
    char buf[17];

    number_to_string((ulong)&buf, buf, sizeof(buf), 0x10);

    fc_puts(scr, (byte *)label, 20, 0, 0x5f);
    fc_puts(scr, (byte *)buf, 20 + sizeof(label) - 1, 0, 0x5f);

    wait_clocks(ROUGH_CLOCK_SEC / 4);
    check_stack(scr);
}
*/
/*
void check_malloc(fc_window_t *window) {
    int i, j;
    void *p[5];
    char buf[17];

    typedef struct chunk {
        struct chunk *next;
        size_t size;
    } chunk_t;
    chunk_t *ch;

    fc_wputs(window, "heap_bottom=", 15);
    number_to_string((ulong)&_heap_bottom, buf, sizeof(buf), 0x10);
    fc_wputs(window, buf, 15);
    fc_wputs(window, "\n", 15);

    for (j = 0; j < 3; j++) {
        for (i = 0; i < 5; i++) {
            p[i] = malloc(32 * i + 1);
            ch = (chunk_t *)p[i] - 1;

            number_to_string((ulong)p[i], buf, sizeof(buf), 0x10);
            fc_wputs(window, buf, 15);
            fc_wputs(window, " (", 15);
            number_to_string(ch->size, buf, sizeof(buf), 0x10);
            fc_wputs(window, buf, 15);
            fc_wputs(window, ", ", 15);
            number_to_string((ulong)ch->next, buf, sizeof(buf), 0x10);
            fc_wputs(window, buf, 15);
            fc_wputs(window, ")\n", 15);
        }

        for (i = 4; i >= 0; i--)  {
            free(p[i]);
        }
    }
}*/

int main() {
    fc_screen_t scr = create_cga_screen();
    fc_window_t win = create_window(&scr, 1, 1, scr.width - 2, scr.height - 2);

    box_screen(&scr, 11);
    fc_move_cursor(&scr, win.x, win.y);

	file_t inp;
    ulong size;
    long i;
    char buf[128];
    key_state_t state = {
        .lshift = 0,
        .rshift = 0,
    };

    pid_t pid = fexec("/prgm/scheme", 0, NULL, 0);

    char msg[128];
    ulong msg_size;
    next_message_t next = 0;

	for (;;) {
        /* Output to screen from child stdout/stderr */
        do {
            msg_size = proc_recv_msg(msg, sizeof(msg), &next);
            if (msg_size > 0 && msg_size <= sizeof(msg)) {
                for (i = 0; i < (long)msg_size; i++) {
                    fc_wputc(&win, msg[i], 15);
                }
                proc_consume_msg(&next);
            }
            if (msg_size > sizeof(msg)) {
                // FIXME
                fc_wputs(&win, "-Discarding-", 5);
                proc_consume_msg(&next);
            }
        } while (msg_size != 0);
        proc_stop_recv(&next);

        /* Input to child stdin */
	    inp = fs_acquire("/dev/kb", 0);
		if (inp && (size = fs_size(inp))) {
            if (size + 1 > sizeof(buf)) {
                // FIXME
                fc_wputs(&win, "\nToo much input!\n", 15);
                break;
            }

            fs_read((byte *)buf, 0, size, inp);
            for (i = size - 1; i >= 0; i--) {
                process_key(buf[i], &win, &state, pid);
            }
		}
	    fs_release(inp);
        wait_clocks(ROUGH_CLOCK_SEC / 8);
	}
    halt();
	return 0;
}

