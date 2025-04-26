#include "variables.h"

extern int variables_checked;
extern int n_errors;
extern int comment_lines_del;
extern int files_included;
extern int n_lines;

char *valid_types[] = { "char", "short", "int", "long", "float", "double", "_Bool", "bool", "unsigned", "signed", "size_t", "intptr_t", "const", "typedef" };
array custom_types;

bool preprocess_variables(char* line){
    char *semicolon;
    int offset;
    bool vars = true;

    while((semicolon = strchr(line, ';')) != NULL) {
        char *temp;
        offset = semicolon-line+1;
        temp = (char *)calloc(offset, sizeof(char));
        strncpy(temp, line, offset); // first part of a string
        vars = vars && check_variables(temp);
        line += offset;
        free(temp);
    }
    return vars;
}


bool check_variables(char* line) {
    char delimiters[] = " ,;";
    bool type_checked = false;
    bool def = false;

    char *token = strtok(line, delimiters);
    while (token != NULL) {
        if (!strcmp(token, "=")) return true; // fine
        if (!strcmp(token, "typedef")) def = true; // defining a new type
        if (type_checked) {
            type_checked = false;
            if (token[0] >= '0' && token[0] <= '9')  {
                n_errors += 1;
            }
            for (int j = 0; j < strlen(token); j++) {
                if (token[j] == '-' || token[j] == '&') {
                    n_errors += 1;
                    break;
                }
            }
        }
        for (int i=0; i<sizeof(valid_types)/sizeof(valid_types[0]); i++) {
            if(!strcmp(valid_types[i], token)) {
                type_checked = true;
                break;
            }
        }
        for (int i=0; i<custom_types.size; i++) {
            if (!strcmp(custom_types.items[i], token)) {
                type_checked = true;
                break;
            }
        }
        if(!type_checked){
            if(!def) {
                printf("variabile! : %s\n", token);
                variables_checked+=1;
            }
            if(def) {
                append(&custom_types, token);
                def = false;
            }
        }
        token = strtok(NULL, delimiters);
    }
    return true;
}  