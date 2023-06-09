#ifndef FCURSES_H
#define FCURSES_H

#include <frosk.h>

enum output_format {
    // cga text mode
    FORMAT_CHAR8_STYLE8 = 0,
};

typedef struct fc_screen fc_screen_t;
typedef struct fc_window fc_window_t;

struct fc_screen {
    int width, height, buf_size;
    file_t file, cursor_file;
    enum output_format format;
};

// TODO scroll back buffer
struct fc_window {
    fc_screen_t *scr;
    int x, y, w, h;
    int pt_x, pt_y;
};

fc_screen_t create_cga_screen(void);
void fc_putc(fc_screen_t *scr, int ch, int x, int y, int style);
void fc_puts(fc_screen_t *scr, const byte *str, int x, int y, int style);
void fc_move_buf(fc_screen_t *scr, int sx, int sy, int dx, int dy, int len);
void fc_move_cursor(fc_screen_t *scr, int x, int y);

fc_window_t create_window(fc_screen_t *scr, int x, int y, int w, int h);
void fc_wputc(fc_window_t *win, int ch, int style);
void fc_wputs(fc_window_t *win, const char *s, int style);

int number_to_string(ulong value, char *str, int max, int radix);

#endif
