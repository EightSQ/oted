#pragma once

#include "line.h"

#define MAX_FILENAME_SIZE 256
#define HALF_BUFFER_SIZE 1048576

typedef enum {
    NO_BUMP, LEFT_BUMP, RIGHT_BUMP, TOP_BUMP, BOTTOM_BUMP
} bump;

typedef enum {
    NEWLINE_BELOW, NEWLINE_ABOVE
} newline_location;

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
extern void insert_newline(file_buffer *fb, newline_location nl);

extern bump left(file_buffer *fb);
extern bump right(file_buffer *fb);
extern bump down(file_buffer *fb);
extern bump up(file_buffer *fb);