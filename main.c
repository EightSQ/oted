#include <stdio.h>
#include <stdlib.h>

#include "file.h"

int main(int argc, char *argv[]) {
    if(argc < 2)
    {
        fprintf(stderr, "Usage: %s filename1 ...", argv[0]);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}