#include "comments.h"

extern bool in_comment;
extern bool multiline_comm;

bool handle_comments(char *line, FILE *buffer) {
	// lunghezza della linea che leggiamo
	//bool in_comment = false;
	char *comment;

	// if line was modified
	bool skip = false;

	// controlliamo se siamo in un commento e prossimi char sono */
	if(in_comment && (comment = strchr(line, '*')) != NULL && comment[1]=='/') {
		in_comment = multiline_comment(line);
	} else if(in_comment) return true; // il commento continua per il resto della linea

	if((comment = strchr(line, '/')) != NULL) {
		if (comment[1]=='/') { 
			if (line[0]=='/') return true; // il commento inizia a  carattere 0, possiamo skippare
			
			int index = (int)(comment - line);
			//strncpy(line, line, index-1);
			//printf("sketchy cp done");
			line[index] = '\0'; // tagliamo la stringa
			in_comment = false; // go to next line
		} else if (comment[1] == '*') { //è stato aperto un multi-line comment
			printf("chiamata multiline_comment\n");
			printf("%s\n", line);
			in_comment = multiline_comment(line);
		}
	}
	return in_comment;
}

// gestisce i commenti multilinea
bool multiline_comment(char *line) {
	// se siamo in un multiline comment
	//bool multiline_comm = false;
    char temp[sizeof(line)];

    int k=0;
	for (int i = 0; i<strlen(line)-1; i++) {
		if (line[i] == '/' && line[i + 1] == '*') {
			multiline_comm = true; // entriamo in multiline comment
			i++;
		}
		else if (line[i] == '*' && line[i + 1] == '/') {
			multiline_comm = false; // usciamo da un multiline comment
			i++;
		//} else if (!multiline_comm) fputc(line[i], buffer); // altrimenti appendiamo carttere a buffer
		} else if (!multiline_comm) temp[k++]=line[i];
	}
	temp[k]='\0';
	strcpy(line, temp);
	return multiline_comm; // comunica se la prossima riga è un commento
}