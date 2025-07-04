#include "variables.h"

extern int variables_checked;
extern int n_errors;
extern int comment_lines_del;
extern int files_included;
extern int n_lines;

char *valid_types[] = { "char", "char*", "short", "int", "long", "float", "double", "_Bool", "bool", "unsigned", "signed", "size_t", "intptr_t", "const", "typedef", "enum", "struct" };
array custom_types;
bool in_enum = false;
bool in_struct = false;

const char* keywords[] = {
    "if", "else", "switch", "case", "default",
    "while", "do", "for", "break", "continue", "goto", "return",
    "struct", "union", "enum", "typedef", "sizeof", "void", "inline"
};

// takes line, splits it on ';' and sends instructions to check_variables()
// returns if any variables were declared in the current line
bool preprocess_variables(char* line, array *errors, int line_num, char *file_name){
    char *semicolon;
    int offset;
    bool vars = true;
    
    //handles defines
    if(strchr(line, ';') == NULL) {
        if (strstr(line, "#define") != NULL) //huh
            variables_checked++;
    }

    //handles if current line is enum closing
    if (in_enum && strchr(line, ';') != NULL) {
        in_enum = false;
        return vars;
    } else if (in_enum) return vars;
    
    // handles whether current line is struct closing
    if (in_struct && strstr(line, "};") != NULL) {
        in_struct = false;
        return vars;
    }

    //handles enums
    char* enum_idx;
    if((enum_idx = strstr(line, "enum"))!=NULL && line-enum_idx == 0) { //if enum starts @ index 0 (otherwise problems with words that contain `enum`)
        in_enum = true;
        vars = vars && check_variables(line, errors, line_num, file_name);
        if (strchr(line, ';') != NULL) in_enum=false; //enum in one line !
        return vars;
    }

    //handles structs
    char* struct_idx;
    if((struct_idx = strstr(line, "struct"))!=NULL && line-struct_idx == 0) { //if struct starts @ index 0
        in_struct = true;
        vars = vars && check_variables(line, errors, line_num, file_name);
        if (strchr(line, ';') != NULL) { // check for declarations inline
            line = strchr(line, '{')+1;
        } else if (strchr(line, '}') != NULL) { // struct in one line (TODO: non proprio, `;` potrebbe essere alla riga successiva)
            in_struct=false; //prima era in_enum = false
            return vars;
        } else return vars;
    }
    
    // check for multiple regular instructions in one line
    while((semicolon = strchr(line, ';')) != NULL) {
        char *temp;
        offset = semicolon-line+1;
        temp = (char *)calloc(offset, sizeof(char));
        strncpy(temp, line, offset); // copying instruction
        vars = vars && check_variables(temp, errors, line_num, file_name);
        line += offset;
        free(temp);
    }
    //TODO: secondo me da togliere
    //free(semicolon);
    //free(enum_idx);
    return vars;
}

// takes an istruction as an input, iterates to the vame of the variable (if present) and checks for its validity
// returns if any variables were declared in the current instruction
bool check_variables(char* line, array *errors, int line_num, char *file_name) {

    // if it's an array, extract its name
    if (is_array(line)) {
        printf("array found!!\n");
        // looks for the last [ opening (end of variable name)
        int last = 0;
        for(int i=0; i<strlen(line); i++){
            if(line[i] == '['){
                last = i;
            }
        }
        // the variable is cut off, keeping only its name
        line[last] = '\0';
    }

    char *temp = (char *)calloc(strlen(line), sizeof(char));
    temp=strcpy(temp, line);
    char *token = strtok(temp, " ");
    int skip_len=0;
    bool def = false;
    if(token == NULL) return true; // no tokens are found
    while (strchr(token, '\t') != NULL){ // removing left tabs
        token++;
    }

    bool found_var=false; // current instruction has a variable declaration
    bool type = false; // if the current token is a type (regular or special)
    do { // iterate to the first token that is not a type
        bool foundtype = false;
        for (int i=0; i<sizeof(valid_types)/sizeof(valid_types[0]); i++) {
            if(!strcmp(valid_types[i], token)) {
                foundtype = true;
                if(!strcmp(valid_types[i], "typedef") || !strcmp(valid_types[i], "enum") 
                || (!strcmp(valid_types[i], "struct"))){
                    def = true;
                }
            }
        }
        for (int i=0; i<custom_types.size; i++) {
            if(!strcmp(custom_types.items[i], token)) {
                foundtype = true;
            }
        }
        if (foundtype) {
            found_var = true;
            token = strtok(NULL, " ");
            skip_len += strlen(token);
            type = true;
        } else type = false;
    } while (type && token != NULL);

    char *token_copy = strdup(token);
    printf("line:%s, token:%s\n",line,token_copy);
    line = strstr(line, token_copy); // copy rest of line (no types) to line
    free(temp);
    
    if(def) {
		// we use ';' for typedef
        token_copy[strcspn(token_copy, ";{")]='\0';

        // add custom type to list
        append(&custom_types, token_copy);
        def = false;
        check_error(token_copy,errors,file_name,line_num);
        free(token_copy);
        return found_var;
    }
    char *var_name;
    char* newline = strip(line); // strip line of spaces

    if (strchr(newline, ',')!=NULL && found_var) { // declaration of multiple variables in a single instruction
        var_name = strtok(newline, ",");

        while (var_name != NULL) {
            check_error(var_name,errors,file_name,line_num);
            variables_checked++;
            var_name = strtok(NULL, ",");
        }
    }else if (found_var) {
        check_error(newline,errors,file_name,line_num);
        variables_checked++;
    }
    free(newline);
    return found_var;
}

// logs error
void handle_error(array *errors, char *file_name, int line_num) {
    n_errors++;
    int size = snprintf(0, 0, "error #%d: %s:%d\n", n_errors, file_name, line_num)+1;
    char *temp = (char *)calloc(size, sizeof(char));
    snprintf(temp, size, "error #%d: %s:%d\n", n_errors, file_name, line_num);
    append(errors, temp);
	free(temp);
}

// checks if a variable name is legal
void check_error(char *var_name, array *errors, char *file_name, int line_num) {
    char *valid = var_name;
    while (*valid == '*') {
        valid++;
    }
    memmove(var_name, valid, strlen(valid)+1);  // +1 per copiare anche '\0'

    //printf("checking, %s\n", var_name);

    if (strchr(var_name, ';')!=NULL) {
        var_name[strlen(var_name)-1]='\0';
    }
    if ((var_name[0] >= '0' && var_name[0] <= '9')) {
        handle_error(errors, file_name, line_num);
    }

    // keep just the variable name, by removing everything after the first equals
    if (strchr(var_name, '=')!=NULL) {
        var_name = strtok(var_name, "=");
    }

    if (strcspn(var_name, " !@#$%^()[]{}+-/\\|:?><~&*") != strlen(var_name)) {
        printf("%s\n", var_name);
        handle_error(errors, file_name, line_num);
    }
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        if (!strcmp(keywords[i], var_name)) {
            handle_error(errors, file_name, line_num);
            break;
        }
    }
}

// strips a line of its spaces
char* strip(char* line) {
    int n = strlen(line);
    char *new = (char *)calloc(n+1,sizeof(char));

    int j = 0;
    for(int i=0; i<n;i++){
        char prec = i == 0 ? ';' : line[i-1];
        char succ = i == n-1 ? ' ' : line[i+1];

        // if the current character is a space that needs to be removed or a \t or a \n, it gets skipped
        if(!((line[i] == ' ' && is_removable(prec, succ)) || line[i] == '\t' || line[i] == '\n')) {
            new[j++] = line[i];
        }
    }
    new[j] = '\0';
    return new;
}

// a space is removable if it's followed or preceded by one of these characters
// ex: "var = 5" would become "var=5"
bool is_removable(char pre, char post) {
    return (pre == '=' || pre == ';' || pre == ' ' || pre == ',' ||
    post == '=' || post == ';' || post == ' ' || post == ',' );    
}

// checks whether the current variable matches an array declaration or initialisation
bool is_array(const char *str) {
    printf("checking: %s\n", str);
    regex_t regdecl, reginit;

    // checks for [] with any number of digits in it, followed by optional spaces and a ;
    const char *array_decl = "\\[[0-9]*\\]\\s*;";
    // checks for [] with any number of digits in it, followed by optional spaces
    // and an =, then a {} containing anything, then optional spaces and a ;
    // ( '[^}]' = any character that isn't '}')
    const char *array_init = "\\[[0-9]*\\]\\s*=\\s*\\{[^}]*\\}\\s*;";
    
    // compiles the regexes
    if (regcomp(&regdecl, array_decl, REG_EXTENDED) || regcomp(&reginit, array_init, REG_EXTENDED)) {
        fprintf(stderr, "Error compiling RegEx");
        return false;
    }

    // regexec returns 0 if match is found
    int decl = regexec(&regdecl, str, 0, NULL, 0);
    //printf("%s, decl match: %d\n", str, res1);
    int init = regexec(&reginit, str, 0, NULL, 0);
    //printf("%s, init match: %d\n", str, res2);
    regfree(&regdecl);
    regfree(&reginit);

    // can't do simple boolean logic because regexec can return numbers other than 0 and 1
    return ( (decl == 0) != (init == 0) ); // essentially a xor
}