#include "comments.c"
#include "globals.h"
#include "errors.c"

void read_file(char *input, FILE *buffer);

int main(int argc, char *argv[]) {
	// eseguito senza parametri
	variables_checked = 0;
	n_errors = 0;
	comment_lines_del = 0;
	if (argc == 1)
		return 1; // no input file
	int i = 1;

	char *input;
	char *output;
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
			printf("input: %s \n", input);
			// eseguito con long flag per file di input
		} else if (!strncmp(argv[i], "--in", 4)) {
			input = (char *)calloc(strlen(argv[i]) - 5, sizeof(char));
			strcpy(input, &argv[i++][5]);
			printf("input: %s \n", input);
			// eseguito con flag per file di output
		} else if (!strcmp(argv[i], "-o")) {
			output = (char *)calloc(strlen(argv[++i]), sizeof(char));
			strcpy(output, argv[i++]);
			printf("output: %s \n", output);
			// eseguito con long flag per file di output
		} else if (!strcmp(argv[i], "--out")) {
			output = (char *)calloc(strlen(argv[i]) - 6, sizeof(char));
			strcpy(output, &argv[i++][6]);
			printf("output: %s \n", output);
			// eseguito con flag verbose
		} else if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")) {
			verbose = true;
			printf("verboso ! \n");
		}
		else {
			return 0;
		}
	}
	printf("%s\n", input);
	FILE *buffer;
	buffer = fopen("buffer.temp", "w");
	read_file(input, buffer);
}

void read_file(char *input, FILE *buffer) {
	fseek(buffer, 0, SEEK_END);
	FILE *file_in;
	file_in = fopen(input, "r");
	// file input non valido
	if (file_in == NULL) {
		printf("errore in apertura\n");
		exit(1);
	}

	char *line = NULL;
	// dimensione di line (se è 0, la ignora e alloca da solo il buffer)
	size_t len = 0;

	// lunghezza della linea che leggiamo
	ssize_t line_len;

	// bool in_comment = false;
  //getline(&buffer, &size, stdin)
	while ((line_len = getline(&line, &len, file_in)) != -1) {
		// char *comment;

		// recursively append all includes in the file
		if (line[0] == '#' && strstr(line, "include")!= NULL) {
			char *lib_name;
			int lib_len = line_len - 11;
			if (line[line_len - 1] == '\n') lib_len--;
			lib_name = (char *)calloc(lib_len, sizeof(char));

      // char *strncpy(char *str, const char *str2, size_t count)
      // why this & ?
			strncpy(lib_name, &line[10], lib_len);
			printf("%s\n", lib_name);
			read_file(lib_name, buffer);
			continue;
		}
		// if the line doesn't need to be checked (for variable syntax, etc)
		int skip = handle_comments(line, buffer);

		if(!skip){
			check_variables(line);
			fputs(line, buffer);
		} // altrimenti viene skippata
	}
	free(line);
}
// 0: non modificata, 1: commento nella linea, -1: skip
