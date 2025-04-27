#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "globals.c"

void append_lib(char *path, FILE *buffer);
void append_inline(char *line, char *comment, FILE *buffer);
bool multiline_comment(char *line);
bool handle_comments(char* line, FILE *buffer);