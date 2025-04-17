#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void read_file(char* input);
void append_lib(char *path, FILE *buffer);
void append_inline(char *line, char *comment, FILE *buffer);

int main(int argc, char *argv[]) {
	if (argc == 1)
		return 1; // no input file
	int i = 1;

	char *input;
	char *output;
	bool verbose = false;

	while (i < argc) {
		if (argv[1][0] != '-') {
			input = (char *)calloc(strlen(argv[i]), sizeof(char));
			strcpy(input, argv[i]);
			break;
		}
		if (!strcmp(argv[i], "-i")) {
			input = (char *)calloc(strlen(argv[++i]), sizeof(char));
			strcpy(input, argv[i++]);
			printf("input: %s \n", input);
		} else if (!strncmp(argv[i], "--in", 4)) {
			input = (char *)calloc(strlen(argv[i])-5, sizeof(char));
			strcpy(input, &argv[i++][5]);
			printf("input: %s \n", input);
		} else if (!strcmp(argv[i], "-o")) {
			output = (char *)calloc(strlen(argv[++i]), sizeof(char));
			strcpy(output, argv[i++]);
			printf("output: %s \n", output);
		} else if (!strcmp(argv[i], "--out")) {
			output = (char *)calloc(strlen(argv[i])-6, sizeof(char));
			strcpy(output, &argv[i++][6]);
			printf("output: %s \n", output);
		} else if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")) {
			verbose = true;
			printf("verboso ! \n");
		}
		else {
			return 0;
		}
	}
	printf("%s\n", input);
	read_file(input);

}

void read_file(char* input) {
	FILE *file_in;
	FILE *buffer;
	buffer = fopen("buffer.temp", "w");
	file_in = fopen(input,"r");
	if (file_in==NULL) {
		printf("errore in apertura\n");
		exit(1);
	}

	char *line = NULL;
    size_t len = 0;
    ssize_t line_len;
	
	while ((line_len = getline(&line, &len, file_in)) != -1 ){
		//printf("retrieved line of length %zu:\n", line_len);
        //printf("%s \n", line);
		char *comment;
		
		// fucking comment
		if((comment = strchr(line, '/')) != NULL){
			if (comment[1]=='/') { 

				append_inline(line, comment, buffer);
				

			} else if (comment[1]== '*') { 

				//controllare anche la riga stessa
				// ciclare sulla riga stessa e poi
				// 
			}
			
		}
		// including library
		if (line[0]=='#') {
			char *lib;
			int lib_len = line_len - 11;
			if (line[line_len-1]=='\n') lib_len--;
			lib = (char *)calloc(lib_len, sizeof(char));
			strncpy(lib, &line[10], lib_len);
			printf("%s\n", lib);
			append_lib(lib, buffer);
		}

	}
	free(line);
}

void append_lib(char *path, FILE *buffer) {
	FILE *library;
	library = fopen(path, "r");
	fseek(buffer, 0, SEEK_END); // posiziona il puntatore alla fine
	char *line = NULL;
    size_t len = 0;
    ssize_t read;
	
	while ((read = getline(&line, &len, library)) != -1){
		fputs(line, buffer);
	}
	fputs("\n", buffer);
}

void append_inline(char *line, char *comment, FILE *buffer) {
	int index = (int)(comment - line);
	char cutline[index];
	strncpy(cutline, line, index-1);
	fseek(buffer, 0, SEEK_END); // posiziona il puntatore alla fine
	fputs(cutline, buffer);
	fputs("\n", buffer);
}