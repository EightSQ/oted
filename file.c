#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"

/* static function declarations */
static unsigned int open_line_length(open_line_buffer *olb);
static unsigned char buf_left(open_line_buffer *olb);
static unsigned char buf_right(open_line_buffer *olb);
static void destroy_open_line(open_line_buffer *olb);

static file_buffer *create_file_buffer(void);
static void add_line(file_buffer *fb, char *str);
static void stage_open_line(file_buffer *fb);
static void open_current_line(file_buffer *fb);
static void insert_newline_below(file_buffer *fb);
static void insert_newline_above(file_buffer *fb);


static unsigned int open_line_length(open_line_buffer *olb)
{
    return olb->lfront + (HALF_BUFFER_SIZE - olb->rfront);
}

static unsigned char buf_left(open_line_buffer *olb)
{
    if(olb->lfront == 0 || olb->rfront == 0)
        /* second boolean part: controlled fail behaviour if right open_file_buffer is full */
        return LEFT_BUMP;
    olb->lfront--;
    olb->rfront--;
    olb->right[olb->rfront] = olb->left[olb->lfront];
    return NO_BUMP;
}

static unsigned char buf_right(open_line_buffer *olb)
{
    if(olb->lfront == HALF_BUFFER_SIZE || olb->rfront == HALF_BUFFER_SIZE)
        /* first boolean part: controlled fail behaviour if left open_file_buffer is full */
        return RIGHT_BUMP;
    olb->right[olb->lfront] = olb->left[olb->rfront];
    olb->lfront++;
    olb->rfront++;
}

static void stage_open_line(file_buffer *fb)
{
    unsigned int len = open_line_length(fb->open);
    enlarge_line_buffer(fb->open->open_line_stage_buffer, len);

    unsigned int num_left_bytes = fb->open->rfront * sizeof(unsigned int);
    unsigned int num_right_bytes = (HALF_BUFFER_SIZE - fb->open->rfront) * sizeof(unsigned int);
    memcpy(fb->open->open_line_stage_buffer->characters, fb->open->left, num_left_bytes);
    memcpy(fb->open->open_line_stage_buffer->characters + num_left_bytes, fb->open->right + fb->open->rfront, num_right_bytes);
}

static void open_current_line(file_buffer *fb)
{
    line_buffer *lb = fb->current_line->content;
    if(lb->filled < fb->cursor_pos)
        fb->cursor_pos = lb->filled;

    fb->open->lfront = fb->cursor_pos;
    fb->open->rfront = HALF_BUFFER_SIZE - lb->filled + fb->cursor_pos;
    memcpy(fb->open->left, lb->characters, fb->cursor_pos * sizeof(unsigned int));
    memcpy(fb->open->right + fb->open->rfront, lb->characters+fb->cursor_pos, (lb->filled - fb->cursor_pos) * sizeof(unsigned int));
}

static void insert_newline_below(file_buffer *fb)
{
    /* create new line */
    line *new = create_line(fb->current_line, fb->current_line->next);

    /* put it into the double linked list */
    if(fb->current_line->next != NULL)
        ((line*)(fb->current_line->next))->prev = new;
    fb->current_line->next = new;

    down(fb);
}

static void insert_newline_above(file_buffer *fb)
{
    /* create new line */
    line *new = create_line(fb->current_line, fb->current_line->next);

    /* put it into the double linked list */
    if(fb->current_line->prev != NULL)
        ((line*)(fb->current_line->prev))->next = new;
    else /* if we are at top, we need to remark the first element */
        fb->first_line = new;
    fb->current_line->prev = new;

    up(fb);
}

static void destroy_open_line(open_line_buffer *olb)
{
    free(olb->open_line_stage_buffer);
    free(olb->left);
    free(olb->right);
    free(olb);
}

extern void close_file(file_buffer *fb)
{
    write_file(fb);
    destroy_file_buffer(fb);
}

extern void destroy_file_buffer(file_buffer *fb)
{
    /* destruct all lines */
    line *n;
    line *l = fb->first_line;
    do {
        n = (line*)(l->next);
        destroy_line(l);
    } while (n != NULL);

    /* destruct other data fields */
    destroy_open_line(fb->open);
    free(fb->filename);
    free(fb);
}

extern void write_file(file_buffer *fb)
{
    line *l = fb->first_line;
    FILE *fd = fopen(fb->filename, "w");

    unsigned int *line_set;
    do {
        /* put every unicode character one by one */
        line_set = l->content->characters;
        if(l == fb->current_line) /* use open line stage buffer instead for currently open line */
        {
            stage_open_line(fb);
            line_set = fb->open->open_line_stage_buffer->characters;
        }

        for(int i = 0; i < l->content->filled; i++)
            putc(line_set[i], fd);

        /* next line for next iteration */
        l = (line*)(l->next);
    } while (l != NULL);

    fclose(fd);
}

extern void insert_newline(file_buffer *fb, unsigned char location)
{
    switch(location)
    {
        case NEWLINE_ABOVE:
            insert_newline_above(fb);
            break;
        case NEWLINE_BELOW:
        default:
            insert_newline_below(fb);
            break;
    }
}

extern unsigned char left(file_buffer *fb)
{
    return buf_left(fb->open);
}

extern unsigned char right(file_buffer *fb)
{
    return buf_right(fb->open);
}

extern unsigned char down(file_buffer *fb)
{
    if(fb->current_line->next == NULL)
        return BOTTOM_BUMP;

    /* construct line_buffer for current line */
    stage_open_line(fb);

    /* swap line_buffer pointers */
    line_buffer *updated_line_buffer = fb->open->open_line_stage_buffer;
    fb->open->open_line_stage_buffer = fb->current_line->content;
    fb->current_line->content = updated_line_buffer;

    /* move down to next line */
    fb->current_line = (line*)(fb->current_line->next);
    open_current_line(fb);
    return NO_BUMP;
}

extern unsigned char up(file_buffer *fb)
{
    if(fb->current_line->prev == NULL)
        return TOP_BUMP;

    /* construct line_buffer for current line */
    stage_open_line(fb);

    /* swap line_buffer pointers */
    line_buffer *updated_line_buffer = fb->open->open_line_stage_buffer;
    fb->open->open_line_stage_buffer = fb->current_line->content;
    fb->current_line->content = updated_line_buffer;

    fb->current_line = (line*)(fb->current_line->prev);
    open_current_line(fb);
    return NO_BUMP;
}