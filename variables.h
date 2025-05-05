#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "globals.c"
#include "arrays.c"

bool check_variables(char* line, array *errors, int line_num, char *file_name);
bool preprocess_variables(char* line, array *errors, int line_num, char *file_name);
void handle_error(array *errors, char *file_name, int line_num);