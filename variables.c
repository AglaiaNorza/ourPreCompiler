#include "variables.h"

extern int variables_checked;
extern int n_errors;
extern int comment_lines_del;
extern int files_included;
extern int n_lines;

char *valid_types[] = { "char", "short", "int", "long", "float", "double", "_Bool", "bool", "unsigned", "signed", "size_t", "intptr_t", "const", "typedef" };
array custom_types;

const char* keywords[] = {
    "if", "else", "switch", "case", "default",
    "while", "do", "for", "break", "continue", "goto", "return",
    "struct", "union", "enum", "typedef", "sizeof", "void", "inline"
};

bool preprocess_variables(char* line, array *errors, int line_num, char *file_name){
    char *semicolon;
    int offset;
    bool vars = true;

    while((semicolon = strchr(line, ';')) != NULL) {
        char *temp;
        offset = semicolon-line+1;
        temp = (char *)calloc(offset, sizeof(char));
        strncpy(temp, line, offset); // first part of a string
        vars = vars && check_variables(temp, errors, line_num, file_name);
        line += offset;
        free(temp);
    }
    return vars;
}


bool check_variables(char* line, array *errors, int line_num, char *file_name) {
    char delimiters[] = " ,;";
    bool type_checked = false;
    bool def = false;
    bool found_var = false;

    char *token = strtok(line, delimiters);
    int i = 0;

    while (strchr(token, '\t') != NULL){ // removing trailing tabs
        token++;
    } 
    while (token != NULL) {
        if (!strcmp(token, "=")) return found_var; // fine
        if (!strcmp(token, "typedef")) def = true; // defining a new type
        if (type_checked) { // controlliamo nome di una variabile
        
            for (int i=0; i<sizeof(keywords)/sizeof(keywords[0]); i++) {
                if(!strcmp(keywords[i], token)) {
                   handle_error(errors, file_name, line_num);
                   break;
                }
            }
           
            type_checked = false;
            if (token[0] >= '0' && token[0] <= '9')  {
                handle_error(errors, file_name, line_num);
            }
            for (int j = 0; j < strlen(token); j++) {
                if (token[j] == '-' || token[j] == '&') {
                    handle_error(errors, file_name, line_num);
                    break;
                }
            }
        }

        for (int i=0; i<sizeof(valid_types)/sizeof(valid_types[0]); i++) {
            if(!strcmp(valid_types[i], token)) {
                type_checked = true; //abbiamo trovato un tipo
                found_var = true;
                break;
            }
        }
        for (int i=0; i<custom_types.size; i++) {
            if (!strcmp(custom_types.items[i], token)) {
                type_checked = true; //abbiamo trovato un tipo (custom)
                found_var = true;
                break;
            }
        }
        
        if(!type_checked && found_var){
            if(!def) { // abbiamo appena controllato il nome di una variabile
                variables_checked++;
            }
            if(def) { //abbiamo appena controllato un tipo
                append(&custom_types, token);
                def = false;
            }
        }
        token = strtok(NULL, delimiters);
    }
    return found_var;
}

void handle_error(array *errors, char *file_name, int line_num) {
    n_errors++;
    int size = snprintf(0, 0, "error #%d: %s:%d\n", n_errors, file_name, line_num)+1;
    char *temp = (char *)calloc(size, sizeof(char));
    snprintf(temp, size, "error #%d: %s:%d\n", n_errors, file_name, line_num);
    append(errors, temp);
    
}