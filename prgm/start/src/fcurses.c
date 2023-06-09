#include "fcurses.h"

static int format_width[] = {
    2,
};

fc_screen_t create_cga_screen(void) {
    // Default cga screen
	file_t out = fs_acquire("/dev/tvb", 0);
    file_t cursorf = fs_acquire("/dev/cursor", 0);
    return (fc_screen_t){
        .width = 80,
        .height = 25,
        .buf_size = 80 * 25 * 2,
        .file = out,
        .cursor_file = cursorf,
        .format = FORMAT_CHAR8_STYLE8,
    };
}

void fc_putc(fc_screen_t *scr, int ch, int x, int y, int style) {
    byte buf[2];
    int pos = (x + y * scr->width) * format_width[scr->format];

    if (pos >= 0 && pos <= scr->buf_size) {
        switch (scr->format) {
        case FORMAT_CHAR8_STYLE8:
            buf[0] = ch;
            buf[1] = style;
            fs_write(buf, pos, 2, scr->file);
            break;
        }
    }
}

void fc_puts(fc_screen_t *scr, const byte *str, int x, int y, int style) {
#define MAX 512
    int i;
    byte buf[MAX * 2];
    int pos = (x + y * scr->width) * format_width[scr->format];

    if (pos >= 0) {
        // TODO other formats besides FORMAT_CHAR8_STYLE8

        for (i = 0; i < MAX && str[i]; i++) {
            buf[i * 2] = str[i];
            buf[i * 2 + 1] = style;
        }

        if (pos + i > scr->buf_size)
            i = scr->buf_size - pos;

        fs_write(buf, pos, i * 2, scr->file);
    }
#undef MAX
}

void fc_move_buf(fc_screen_t *scr, int sx, int sy, int dx, int dy, int len) {
#define MAX 1024
    int dpos, spos;
    byte buf[MAX];

    dpos = (dx + dy * scr->width) * format_width[scr->format];
    spos = (sx + sy * scr->width) * format_width[scr->format];
    len *= format_width[scr->format];

    // FIXME
    if (len > MAX) len = MAX;

    fs_read(buf, spos, len, scr->file);
    fs_write(buf, dpos, len, scr->file);
#undef MAX
}

void fc_move_cursor(fc_screen_t *scr, int x, int y) {
    struct {
        int x;
        int y;
    } cursor = {
        .x = x,
        .y = y,
    };

    fs_write((byte *)&cursor, 0, sizeof(cursor), scr->cursor_file);
}

fc_window_t create_window(fc_screen_t *scr, int x, int y, int w, int h) {
    return (fc_window_t){
        .scr = scr,
        .x = x,
        .y = y,
        .w = w,
        .h = h,
        .pt_x = 0,
        .pt_y = 0,
    };
}

static void scroll_check(fc_window_t *win) {
    int i;

    if (win->pt_x >= win->w) {
        win->pt_x %= win->w;
        win->pt_y += 1;
    }

    while (win->pt_y >= win->h) {
        // Scroll lines
        for (i = 1; i < win->h; i++) {
            fc_move_buf(
                win->scr,
                win->x, win->y + i,
                win->x, win->y + i - 1,
                win->w
            );
        }

        // Clear the last line
        for (i = 0; i < win->w; i++) {
            fc_putc(win->scr, 0, win->x + i, win->y + win->h - 1, 0);
        }

        win->pt_y -= 1;
    }
}

void fc_wputc(fc_window_t *win, int ch, int style) {
    if (ch == '\n') {
        win->pt_x = 0;
        win->pt_y += 1;
    } else if (ch == '\b') {
        if (win->pt_x > 0) {
            win->pt_x -= 1;
            fc_putc(win->scr, 0, win->x + win->pt_x, win->y + win->pt_y, style);
        }
    } else {
        fc_putc(win->scr, ch, win->x + win->pt_x, win->y + win->pt_y, style);
        win->pt_x += 1;
    }
    scroll_check(win);
    fc_move_cursor(win->scr, win->x + win->pt_x, win->y + win->pt_y);
}

void fc_wputs(fc_window_t *win, const char *s, int style) {
    while (*s) {
        fc_wputc(win, *s, style);
        ++s;
    }
}

int number_to_string(ulong value, char *str, int max, int radix) {
    int i, len, digit;
    ulong v;

    if (value == 0) {
        if (max < 2) return 2;
        str[0] = '0';
        str[1] = 0;
        return 0;
    }

    for (len = 0, v = value; v > 0; v /= radix) len++;
    if (len >= max) return len;

    i = len - 1;
    while (value && i >= 0) {
        digit = value % radix;
        str[i] = digit >= 10 ? 'A' + digit - 10 : '0' + digit;
        value /= radix;
        i--;
    }

    str[len] = 0;
    return 0;
}
