# documentation for the 1st Operating Systems 2 course: ourPreCompiler

## main

after storing the input arguments, the function `read_file()` is called on the input file

* `read_file()` is responsible for reading each line of the input file, and:

  * recursively appending (writing bottom-up) all the `#include` directives from the input file
  * handling comments, in order to check whether it's possible to skip variable checks for one or more lines
  * checking if the `main()` function is entered (to skip checks for the lines involved)
  * calling the function that handles variable declaration checking: `preprocess_variables()`
* the return value is `true` if the file was read correctly, `false` otherwise

## comment handling

### `handle_comments()`

the function checks for both types of comments:

* if it finds a `//` comment, it stores the part of the line that is not part of the comment (to its left), in order to write it to the output
* if it finds a `/* ... */` comment, it calls the function `multiline_comments()`
  the function modifies the variable `line`, which is received as input and then inserted into the output code in `read_file()`

### `multiline_comments()`

the function handles the start and end of multiline comments:

* it iterates over each character of the input line
* if the comment is closed (which does not prevent other comments of the same/different type from being opened within the same line), it stores the characters outside the comment
* it handles the case in which the entire line can be skipped

## variable declaration

### `preprocess_variables()`

the function takes a line of code as input, and, in order:

* handles `#define`s, if present
* handles the end of enum/struct declarations
* handles enum/struct declarations
* handles normal instructions, splitting them (if multiple instructions are present in a single line of code), and invoking `check_variables()`

### `check_variables()`

iterates over the words of the instruction, and performs a do-while loop, in which it checks whether the current word is a recognized type, to get to the name of the variable defined in the instruction (which needs to be checked). once the types have been skipped, the function `strip()` is invoked

* `strip()` removes spaces, if they are considered "removable", from the remaining part of the instruction
* if the instruction contains a definition, the new type gets saved in the `custom_types` array
* then, the function `check_error()` is invoked

### `check_error()`

the function takes as input a string containing the name of a variable (and possibly its following assignment), and performs a series of checks on it.

* if the string results positive to one of the checks, the function `handle_error()` is called. the function stores the error 
* if no error is found, the code proceeds normally (the variable name is valid)


# documentazione progetto 1, os2: ourPreCompiler

## main
dopo aver memorizzato gli argomenti di input, viene chiamata la funzione `read_file()` sul file di input
- `read_file()` si occupa leggere ogni riga del file di input, e:
    - appendere, ricorsivamente (scrivendo bottom-up) tutti gli `#include` del file di input
    - gestire i commenti, in modo tale da controllare se è possibile saltare il controllo per variabili per una o più linee
    - controllare se si entra dentro la funzione `main()` (per skippare i controli per le righe coinvolte)
    - chiamare la funzione che gestisce il controllo della dichiarazione delle variabili: `preprocess_variables()`
- il valore di ritorno è `true` se il file è stato letto correttamente, `false` altrimenti

## gestione dei commenti
### `handle_comments()`
la funzione controlla per i due tipi di commenti:
- se trova un commento di tipo `//`, memorizza la parte di riga che non fa parte del commento (quindi alla sua sinistra), per scriverla in output
- se trova un commento di tipo `/* ... */`, chiama la funzione `multiline_comments()` 
la funzione modifica la variabile `line`, che viene ricevuta in input e che viene, in `read_file()`, inserita nel codice di output

### `multiline_comments()`
la funzione gestisce l'inizio e la fine di commenti multilinea:
- itera su ogni carattere della linea in input
- se il commento è stato chiuso (e ciò non pregiudica il fatto che ne possano venire aperti altri, dello stesso/diverso tipo, nella stessa linea), vengnon memorizzati i caratteri al di fuori del commento
- gestisce il caso in cui l'intera linea viene saltata

## dichiarazione di variabili
### `preprocess_variables()`
la funzione prende in input una riga del codice, e, in ordine:
- gestisce i `#define`, se presenti
- gestisce la fine di dichiarazioni di enum/struct
- gestisce la dichirazione di enum/struct
- gestisce le istruzioni normali, separandole (se più istruzioni sono presenti in una riga di codice), e invocando `check_variables()`

### `check_variables()`
itera sulle parole dell'istruzione, e esegue un do-while loop, in cui controlla se la parola corrente è un tipo riconosciuto, per arrivare al nome della variabile definita nell'istruzione. una volta saltati i tipi, viene invocata la funzione `strip()`
- `strip()` rimuove gli spazi, se rimuovibili, dalla parte di istruzione rimanente
dopo aver ottenuto la parte dell'istruzione che non contiene i tipi, senza spazi
- se l'istruzione contiene una definizione, salviamo il nuovo tipo
- se l'istruzione ha una dichirazione di variabile normale:
in entrambi i casi, viene invocata la funzione `check_error()`

### `check_error()`
la funzione prende in input una stringa, che contiene il nome di una variabile (e in caso anche l'assegnazione seguente), e esegue una serie di controlli su essa.
- in caso la stringa risulti positiva ad una dei controlli, viene chiamata la funzione `handle_error()`, che si occupa di memorizzare l'errore trovato
- se non viene trovato nessun errore, il codice prosegue normalmente (il nome della variabile è valido)
