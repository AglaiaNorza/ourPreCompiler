#include "comments.c"
#include "variables.c"

bool read_file(char *input, FILE *buffer);

extern int variables_checked;
extern int n_errors;
extern int comment_lines_del;
extern int files_included;
extern int n_lines;
extern array custom_types;

array stats;
array errors;
int lines_to_skip = 0;
int total_lines = 0;

int main(int argc, char *argv[]) {
	// eseguito senza parametri
	variables_checked = 0;
	n_errors = 0;
	comment_lines_del = 0;

	if (argc == 1)
		return 1; // no input file
	int i = 1;

	char *input;
	char *output = NULL;
	bool verbose = false;

	while (i < argc) {
		// eseguito con un solo parametro: nome del file di input
		if (argv[1][0] != '-') {
			input = (char *)calloc(strlen(argv[i]), sizeof(char));
			strcpy(input, argv[i]);
			break;
		}
		// eseguito con flag per file di input
		if (!strcmp(argv[i], "-i")) {
			input = (char *)calloc(strlen(argv[++i]), sizeof(char));
			strcpy(input, argv[i++]);
			//printf("input: %s \n", input);
			// eseguito con long flag per file di input
		} else if (!strncmp(argv[i], "--in", 4)) {
			input = (char *)calloc(strlen(argv[i]) - 5, sizeof(char));
			strcpy(input, &argv[i++][5]);
			//printf("input: %s \n", input);
			// eseguito con flag per file di output
		} else if (!strcmp(argv[i], "-o")) {
			output = (char *)calloc(strlen(argv[++i]), sizeof(char));
			strcpy(output, argv[i++]);
			//printf("output: %s \n", output);
			// eseguito con long flag per file di output
		} else if (!strncmp(argv[i], "--out", 5)) {
			output = (char *)calloc(strlen(argv[i]) - 6, sizeof(char));
			strcpy(output, &argv[i++][6]);
			//printf("output: %s \n", output);
			// eseguito con flag verbose
		} else if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")) {
			i++;
			verbose = true;
			//printf("verboso ! \n");
		}
		else { return 0; }
	}
	FILE *buffer;
	buffer = output ? fopen(output, "w") : fopen("buffer.temp", "w+");

	// array dinamici :D
	init_array(&errors);
	init_array(&stats);
	init_array(&custom_types);

	if (!read_file(input, buffer)) {
		fclose(buffer);
		if (!output) {
			if (remove("buffer.temp") != 0) {
				printf("Error: Unable to delete the file.\n");
			}
		} else {
			if (remove(output) != 0) {
				printf("Error: Unable to delete the file.\n");
			}
		}
		exit(1);
	}
		
	if (verbose) {
		for (int i=0; i<errors.size; i++){
			printf("%s", errors.items[i]);
		}
		for (int i=0; i<stats.size; i++){
			printf("%s", stats.items[i]);
		}
		int buffer_size = ftell(buffer);
		printf("variables checked: %d\n"
			"number of errors: %d\n"
			"number of comment lines deleted: %d\n"
			"number of includes: %d\n"
			"size of file: %d\n"
			"number of lines: %d\n",
			variables_checked, n_errors, comment_lines_del, files_included, buffer_size, (total_lines-comment_lines_del)
		);

	}

	if (!output) {
		fseek(buffer, 0, SEEK_SET);
		char ch;
		putchar('\n');
		while ((ch = fgetc(buffer)) != EOF) {
			putchar(ch);
		}
		fclose(buffer);
		if (remove("buffer.temp") != 0) {
			printf("Error: Unable to delete the file.\n");
		}
	}
}

bool read_file(char *input, FILE *buffer) {
	int lines_to_skip = 0;
	bool in_comment = false;
	bool multiline_comm = false;

	fseek(buffer, 0, SEEK_END);
	FILE *file_in;
	file_in = fopen(input, "r");
	// file input non valido
	if (file_in == NULL) {
		printf("errore in apertura sul file %s\n",input);
		return false;
	}

	char *line = NULL;
	// dimensione di line (se è 0, la ignora e alloca da solo il buffer)
	size_t len = 0;

	// lunghezza della linea che leggiamo
	ssize_t line_len;
	int line_number = 0; // numero della riga (per statistiche)
	
	while ((line_len = getline(&line, &len, file_in)) != -1) {
		total_lines++;
		line_number++;
		// char *comment;

		// recursively append all includes in the file
		if (line[0] == '#' && strstr(line, "include")!= NULL) {
			files_included++;
			
			char *lib_name;
			char *last = NULL;
			int lib_len = line_len - 11;
			// trova ultima occorrenza di / (trovare input con cartelle)
			if ((last = strrchr(input, '/'))) {
				lib_len += (last-input+1);
			}
			if (line[line_len - 1] == '\n') lib_len--;
			lib_name = (char *)calloc(lib_len, sizeof(char));

      // char *strncpy(char *str, const char *str2, size_t count)
      // why this & ?
			// modifica lib_name in modo tale da includere il percorso relativo se presente
			if (last) {
				strncpy(lib_name, input, last-input+1);
				strncat(lib_name, &line[10], lib_len-(int)(last-input+1));
			} else {
				strncpy(lib_name, &line[10], lib_len);
			}
			read_file(lib_name, buffer);
			continue;
		}

		// if the line doesn't need to be checked (for variable syntax, etc)
		int skip = handle_comments(line, buffer, &in_comment, &multiline_comm);
		
		if(!skip){
			if (lines_to_skip!=-1 && strstr(line, "int main(")) {
				if (strstr(line, "{")) lines_to_skip = 1; // main declaration is in one line
				else lines_to_skip = 2; 
			}
			if (lines_to_skip == 0){
				if (!preprocess_variables(line, &errors, line_number, input)) lines_to_skip = -1;
			} else lines_to_skip--;
			fputs(line, buffer);
		} else { comment_lines_del++; }
	}

	fseek(file_in, 0, SEEK_END); // seek to end of file (to be safe)
	int file_size = ftell(file_in);

	int size = snprintf(0, 0, "file name: %s, file size: %d bytes, number of lines: %d\n", input, file_size, line_number)+1;
    char *temp = (char *)calloc(size, sizeof(char));
    snprintf(temp, size, "file name: %s, file size: %d bytes, number of lines: %d\n", input, file_size, line_number);
    append(&stats, temp);

	//fprintf(errors, "file name: %s, file size: %d bytes, number of lines: %d\n", input, file_size, line_number);
	free(line);
	return true;
}