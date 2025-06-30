#include "comments.h"

// takes as input: a line, 
bool handle_comments(char *line, bool *in_comment, bool *multiline_comm) {
	char *comment;
    
	// if line was modified
	bool skip = false;

    // multiline comment was closed
	if(*in_comment && (comment = strchr(line, '*')) != NULL && comment[1]=='/') {
		*in_comment = multiline_comment(line, multiline_comm);
	} else if(*in_comment) return true; // comment continues for the rest of the line

	if((comment = strchr(line, '/')) != NULL) {
		if (comment[1]=='/') { 
			if (line[0]=='/') return true; // comment starts at index 0, we can skip the whole line 
			
			int index = (int)(comment - line);
			line[index-1] = '\n';
			line[index] = '\0'; // cuts the line to before '//' comment
			*in_comment = false; // go to next line
		} else if (comment[1] == '*') { // multiline comment was opened
			*in_comment = multiline_comment(line, multiline_comm);
		}
	}
	return *in_comment;
}

// handles multiline comments
bool multiline_comment(char *line, bool *multiline_comm) {
    char temp[sizeof(line)];

    int k=0;
    // iterates on every character of the line, checks if line contains multiine start/end
	for (int i = 0; i<strlen(line)-1; i++) {
		if (line[i] == '/' && line[i + 1] == '*') {
			*multiline_comm = true; // now inside of a multiline comment
			i++;
		}
		else if (line[i] == '*' && line[i + 1] == '/') {
			*multiline_comm = false; // now outside of a multiline comment
			i++;
		} else if (!(*multiline_comm)) temp[k++]=line[i]; // save characters inside the line that are outside of comments (they need to be printed)
	}
	if(k == 0) comment_lines_del++; // if no characters were saved, line was entirely skipped
	temp[k]='\0';
	strcpy(line, temp);
	return *multiline_comm; // tells if the next line starts already in a multiline comment
}
