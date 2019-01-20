#pragma once

#include "line.h"

#define MAX_FILENAME_SIZE 256
#define HALF_BUFFER_SIZE 1048576

#define NO_BUMP     0
#define LEFT_BUMP   1
#define RIGHT_BUMP  2
#define TOP_BUMP    3
#define BOTTOM_BUMP 4

#define NEWLINE_BELOW   0
#define NEWLINE_ABOVE   1

typedef struct {
    unsigned int *left;
    unsigned int *right;
    unsigned int lfront;
    unsigned int rfront;
    line_buffer *open_line_stage_buffer;
} open_line_buffer;

typedef struct {
    char *filename;
    line *first_line;
    line *current_line;
    open_line_buffer *open;
    unsigned int cursor_pos;
} file_buffer;

extern file_buffer *open_file(char *filename);
extern void write_file(file_buffer *fb);
extern void close_file(file_buffer *fb);
extern void destroy_file_buffer(file_buffer *fb);

/* normal mode quick commands */
extern void delete_current_line(file_buffer *fb);
extern void insert_newline(file_buffer *fb, unsigned char location);

extern unsigned char left(file_buffer *fb);
extern unsigned char right(file_buffer *fb);
extern unsigned char down(file_buffer *fb);
extern unsigned char up(file_buffer *fb);