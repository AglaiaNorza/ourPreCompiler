#include "errors.h"

const char *valid_types[] = { "char", "short", "int", "long", "float", "double", "_Bool", "bool", "unsigned", "signed", "size_t", "intptr_t" };

bool check_variables(char* line) {
    char delimiters[] = " ,;";

    char *token = strtok(line, delimiters);
    while (token != NULL) {
        printf("Token: %s\n", token);
        token = strtok(NULL, delimiters);
        for (int i=0; i < sizeof(valid_types)/sizeof(valid_types[0]); i++) {
            if(strcmp(valid_types[i], token)) {
                variables_checked+=1;
                if(token[0]>='0' && token[0]<='9') {
                    printf("%s", token);
                    n_errors+=1;
                }
                for (int j=0; j<strlen(token); j++) {
                    if (token[j]=='-' || token[j]=='&'){
                        printf("%s", token);
                        n_errors+=1;
                    }
                }
            }
        }
    }
    printf("%d", n_errors);
    printf("checked: %d", variables_checked);
    return true;
}  
