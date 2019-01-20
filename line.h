#pragma once

#define LINE_BUFFER_SIZE 512

typedef struct {
    unsigned int *characters;
    unsigned char size;
    unsigned int filled;
} line_buffer;

typedef struct {
    void *prev;
    void *next;
    line_buffer *content;
} line;

extern void enlarge_line_buffer(line_buffer *buffer, size_t minnewsize);
extern void shrink_line_buffer(line_buffer *buffer);
extern line *create_line(void *prev, void *next);
extern void destroy_line(line *line);