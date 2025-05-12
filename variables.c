#include "variables.h"

extern int variables_checked;
extern int n_errors;
extern int comment_lines_del;
extern int files_included;
extern int n_lines;

char *valid_types[] = { "char", "short", "int", "long", "float", "double", "_Bool", "bool", "unsigned", "signed", "size_t", "intptr_t", "const", "typedef", "enum", "struct" };
array custom_types;
bool in_enum = false;
bool in_struct = false;

const char* keywords[] = {
    "if", "else", "switch", "case", "default",
    "while", "do", "for", "break", "continue", "goto", "return",
    "struct", "union", "enum", "typedef", "sizeof", "void", "inline"
};

bool preprocess_variables(char* line, array *errors, int line_num, char *file_name){
    char *semicolon;
    int offset;
    bool vars = true;

    if (in_enum && strchr(line, ';') != NULL) {
        in_enum = false;
        return vars;
    } else if (in_enum) {
        return vars;
    }

    if (in_struct && strchr(line, '}') != NULL) {
        in_struct = false;
        return vars;
    }

    if(strchr(line, ';') == NULL) {
        if (strstr(line, "#define") != NULL)
            variables_checked++;
    }
    
    char* enum_ind;
    if((enum_ind = strstr(line, "enum"))!=NULL && line-enum_ind == 0) {
        in_enum = true;
        vars = vars && check_variables(line, errors, line_num, file_name);
        if (strchr(line, ';') != NULL) in_enum=false;
        return vars;
    }

    char* struct_ind;
    if((struct_ind = strstr(line, "struct"))!=NULL && line-struct_ind == 0) {
        in_struct = true;
        vars = vars && check_variables(line, errors, line_num, file_name);
        if (strchr(line, ';') != NULL) {
            return vars;
        } else if (strchr(line, '}') != NULL) {
            in_enum=false;
            return vars;
        }
    }
    

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
    char *temp = (char *)calloc(strlen(line), sizeof(char));
    temp=strcpy(temp, line);
    char *token = strtok(temp, " ");
    int skip_len=0;
    bool type = false, def = false;
    if(token == NULL) return true; // da capire
    while (strchr(token, '\t') != NULL){ // removing trailing tabs
        token++;
    }

    bool found_var=false;
    do { // iterate to the first token that is not a type
        bool foundtype = false;
        for (int i=0; i<sizeof(valid_types)/sizeof(valid_types[0]); i++) {
            if(!strcmp(valid_types[i], token)) {
                foundtype = true;
                found_var = true;
                if(!strcmp(valid_types[i], "typedef") || !strcmp(valid_types[i], "enum") 
                || (!strcmp(valid_types[i], "struct"))){
                    def = true;
                }
            }
        }
        for (int i=0; i<custom_types.size; i++) {
            if(!strcmp(custom_types.items[i], token)) {
                foundtype = true, found_var = true;
            }
        }
        if (foundtype) {
            token = strtok(NULL, " ");
            skip_len += strlen(token);
            type = true;
        } else type = false;
    } while (type && token != NULL);

    char *token_copy = strdup(token);
    line = strstr(line, token); // copy rest of line (no types) to line
    free(temp);
    
    char* newline = strip(line); // strip line of spaces
    if(def) {
        token_copy[strcspn(token_copy, ";{")]='\0';
        //token_copy[strcspn(token_copy, "{")]='\0';
        printf("token (in def): '%s'\n", token_copy);
        append(&custom_types, token_copy);
        def = false;
        check_error(token_copy,errors,file_name,line_num);
        return found_var;
    }
    char *var_name;
    if (strchr(newline, ',')!=NULL && found_var) {
        var_name = strtok(newline, ",");

        while (var_name != NULL) {
            check_error(var_name,errors,file_name,line_num);
            variables_checked++;
            printf("adding: %s\n", var_name);
            var_name = strtok(NULL, ",");
        }
    }
    else if (found_var) {
        printf("newline is: %s\n", newline);
        //enum dovrebbe entrare qui
        check_error(newline,errors,file_name,line_num);
        variables_checked++;
        printf("adding: %s\n", var_name);

        printf("variable is: %s\n", newline);
    }
    return found_var;
    //chiama get_token e controlla se sono variabili e se c'è una virgola va avanti
}

/*bool check_variables_trash(char* line, array *errors, int line_num, char *file_name) {
    char delimiters[] = " ";
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
                if(!strcmp(valid_types[i], "#define")){
                    printf("abbiamo trovato un define !");
                }
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
}*/

void handle_error(array *errors, char *file_name, int line_num) {
    n_errors++;
    int size = snprintf(0, 0, "error #%d: %s:%d\n", n_errors, file_name, line_num)+1;
    char *temp = (char *)calloc(size, sizeof(char));
    snprintf(temp, size, "error #%d: %s:%d\n", n_errors, file_name, line_num);
    append(errors, temp);
	free(temp);
}

// check if a variable name is legal
void check_error(char *var_name, array *errors, char *file_name, int line_num) {
    if (var_name[0] >= '0' && var_name[0] <= '9') {
        handle_error(errors, file_name, line_num);
    }
    for (int j = 0; j < strlen(var_name); j++) {
        if (var_name[j] == '=')
            break; // next variable name
        if (var_name[j] == '-' || var_name[j] == '&' || var_name[j] == '$' || var_name[j] == ' ' ||
            var_name[j] == '#' || var_name[j] == '%' || var_name[j] == '*') {
            handle_error(errors, file_name, line_num);
            break;
        }
    }
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        if (!strcmp(keywords[i], var_name)) {
            handle_error(errors, file_name, line_num);
            break;
        }
    }
}

char* strip(char* line) {
    int n = strlen(line);
    char *new = (char *)calloc(n+1,sizeof(char));

    int j = 0;
    for(int i=0; i<n;i++){
        char prec = i == 0 ? ';' : line[i-1];
        char succ = i == n-1 ? ' ' : line[i+1];

        if(!((line[i] == ' ' && is_removable(prec, succ)) || line[i] == '\t' || line[i] == '\n')) {
            new[j++] = line[i];
        }
    }
    new[j] = '\0';
    //printf("new is: %s\n", new);
    return new;
}

bool is_removable(char pre, char post) {
    return (pre == '=' || pre == ';' || pre == ' ' || pre == ',' ||
    post == '=' || post == ';' || post == ' ' || post == ',' );    
}
