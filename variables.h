#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "regex.h"
#include "globals.c"
#include "arrays.c"

bool check_variables(char* line, array *errors, int line_num, char *file_name);
bool preprocess_variables(char* line, array *errors, int line_num, char *file_name);
void handle_error(array *errors, char *file_name, int line_num);
char* strip(char *line);
bool is_removable(char pre, char post);
void check_error(char *var_name, array *errors, char *file_name, int line_num);
bool is_array(const char *str);