#include "les.h"
#include <term.h>
#include <stdlib.h>

tline_t *tlines2 = NULL;
size_t tlines2_len = 0;
size_t tlines2_size = 0;
tline_t *tlines3 = NULL;
size_t tlines3_len = 0;
size_t tlines3_size = 0;

void move_forward_long (int n) {
    size_t pos = tlines[tlines_len - 1].end_pos;
    get_tlines(tabb->buf, tabb->buf_len, pos, n - tlines_len + 1, &tlines2, &tlines2_len, &tlines2_size);
    tabb->pos = tlines2[tlines2_len - 1].pos;
    int i;
    for (i = 0; i < tlines_len; i++) {
        if (tabb->buf[tlines[i].end_pos - 1] == '\n') {
            tabb->line++;
        }
    }
    for (i = 0; i < tlines2_len - 1; i++) {
        if (tabb->buf[tlines2[i].end_pos - 1] == '\n') {
            tabb->line++;
        }
    }
    draw_tab();
}

void move_forward_short (int n) {
    int m = n > (tlines_len - 1) ? (tlines_len - 1) : n;
    int i;
    for (i = 0; i < m; i++) {
        if (tabb->buf[tlines[i].end_pos - 1] == '\n') {
            tabb->line++;
        }
    }
    for (i = 0; i < tlines_len - m; i++) {
        tlines[i] = tlines[i + m];
    }
    tlines_len = tlines_len - m;
    size_t pos = tlines[tlines_len - 1].end_pos;
    get_tlines(tabb->buf, tabb->buf_len, pos, m, &tlines2, &tlines2_len, &tlines2_size);
    for (i = 0; i < tlines2_len; i++) {
        tlines_len++;
        tlines[tlines_len - 1] = tlines2[i];
    }
    tabb->pos = tlines[0].pos;
    if (m > (lines - line1 - 1) / 3) {
        draw_tab();
    }
    else {
        printf("%s", cursor_up);
        printf("%s", tparm(parm_index, m));
        printf("%s", tparm(cursor_address, lines - 1 - m, 0));
        draw_tab2(m, tlines2, tlines2_len);
        draw_status();
    }
}

void move_forward (int n) {
    if (tlines_len <= 1) {
        return;
    }
    if (n < lines - line1 - 1 || tlines[tlines_len - 1].end_pos == tabb->buf_len) {
        move_forward_short(n);
    }
    else {
        move_forward_long(n);
    }
}

void move_backward (int n) {
    if (tabb->pos == 0) {
        return;
    }
    int i, t, m, n2;
    size_t pos = tabb->pos;
    if (tlines3_size < n) {
        tlines3_size = n;
        tlines3 = realloc(tlines3, tlines3_size * sizeof (tline_t));
    }
    tlines3_len = 0;
    while (1) {
        int prev = prev_line(tabb->buf, tabb->buf_len, pos, 1);
        get_tlines(tabb->buf, pos, prev, 0, &tlines2, &tlines2_len, &tlines2_size);
        if (!tlines2_len) {
            break;
        }
        for (i = tlines2_len - 1; i >= 0; i--) {
            tlines3_len++;
            tlines3[tlines3_len - 1] = tlines2[i];
            if (tlines3_len == n) {
                goto end;
            }
        }
        pos = prev;
    }

    end:
    m = tlines3_len;
    tabb->pos = tlines3[tlines3_len - 1].pos;
    for (i = 0; i < tlines3_len; i++) {
        if (tabb->buf[tlines3[i].end_pos - 1] == '\n') {
            tabb->line--;
        }
    }
    if (m >= (lines - line1 - 1) / 3) {
        draw_tab();
        return;
    }
    for (i = 0; i < m; i++) {
        tlines2[m - i - 1] = tlines3[i];
    }
    tlines2_len = m;

    n2 = (lines - line1 - 1 - m) > tlines_len ? (tlines_len) : (lines - line1 - 1 - m);
    for (i = n2 - 1; i >= 0; i--) {
        tlines[i + m] = tlines[i];
    }
    tlines_len = n2 + m;
    for (i = 0; i < m; i++) {
        tlines[i] = tlines2[i];
    }

    printf("%s", tparm(cursor_address, line1, 0));
    printf("%s", tparm(parm_rindex, m));
    draw_tab2(m, tlines2, tlines2_len);
    draw_status();
}

void move_start () {
    tabb->pos = 0;
    tabb->line = 1;
    tabb->column = 0;
    draw_tab();
}

void move_end () {
    if (tabb->nlines < lines) {
        tabb->pos = 0;
        tabb->line = 1;
        draw_tab();
        return;
    }
    tabb->pos = tabb->buf_len;
    tabb->line = tabb->nlines + 1;
    move_backward(lines - line1 - 1);
}
