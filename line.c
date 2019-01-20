#include <stdio.h>
#include <stdlib.h>

#include "line.h"

/* static function declarations */
static unsigned int size(line_buffer *buffer);
static line_buffer *create_line_buffer(void);
static void resize_line_buffer(line_buffer *buffer, size_t size);
static void destroy_line_buffer(line_buffer *buffer);

static unsigned int size(line_buffer *buffer)
{
    return (unsigned int)buffer->size * LINE_BUFFER_SIZE;
}

static line_buffer *create_line_buffer(void)
{
    unsigned int *data = (unsigned int*)malloc(LINE_BUFFER_SIZE * sizeof(unsigned int));
    line_buffer *new_buf = (line_buffer*)malloc(sizeof(line_buffer));
    if (data == NULL || new_buf == NULL)
        goto alloc_fail;

    new_buf->characters = data;
    new_buf->size = 1;
    new_buf->filled = 0;
    return new_buf;

alloc_fail:
    fprintf(stderr, "Could not allocate memory for a line_buffer.");
    return NULL;
}

extern void enlarge_line_buffer(line_buffer *buffer, size_t minnewsize)
{
    if (minnewsize <= size(buffer))
        return;

    unsigned char newsize = buffer->size;
    minnewsize -= size(buffer);
    while(minnewsize >= 0)
    {
        minnewsize -= LINE_BUFFER_SIZE;
        newsize++;
    }

    resize_line_buffer(buffer, newsize);
}

extern void shrink_line_buffer(line_buffer *buffer)
{
    unsigned int filled = buffer->filled;
    unsigned char newsize = 0;
    while(filled >= 0)
    {
        filled -= LINE_BUFFER_SIZE;
        newsize++;
    }

    resize_line_buffer(buffer, newsize);
}

static void resize_line_buffer(line_buffer *buffer, size_t size)
{
    buffer->characters = realloc(buffer->characters, size * LINE_BUFFER_SIZE);
}

static void destroy_line_buffer(line_buffer *buffer)
{
    free(buffer->characters);
    free(buffer);
}

extern line *create_line(void *prev, void *next)
{
    line *new_line = (line*)malloc(sizeof(line));
    new_line->prev = prev;
    new_line->next = next;
    new_line->content = create_line_buffer();
}

extern void destroy_line(line *line)
{
    destroy_line_buffer(line->content);
    free(line);
}