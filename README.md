# ourPreCompiler
myPreCompiler è un'applicazione che, dato un file contenente del codice C, lo elabora come segue
1) risolve le direttive `#include`, ovvero include il contenuto dei file argomento della direttiva `#include`;
2) controlla se sono state dichiarate variabili con nome non valido, ovvero identificatori non validi (ad es. x-ray, two&four, 5brothers);
3) elimina tutti i commenti;
4) produce un file di output contenente il codice modificato, ovvero il file di input estenso con gli include e senza commenti;
5) produce le statistiche di elaborazione riportate nella sezione "Specifiche"

## Assunzioni
Si può assumere che:
1) i file inclusi mediante la direttiva `#include` siano memorizzati nella CWD
2) il file contenente il codice C fornito come input sia costituito dal solo blocco di codice della funzione main e che non ci siano altre funzioni
3) tutte le variabili locali siano dichiarate all'inizio della funzione main in righe contigue e che le variabili globali siano dichiarate prima del main in righe contigue;
4) i tipi di dato usati nella dichiarazione delle variabili siano corret;
 ;

## Specifiche
### Input
Il programma prevede tre parametri di input:

-  `-i, --in` (notazione doppio trattino) per specificare il file di input
    
- `-o --out` (notazione doppio trattino) per specificare il file di output
    
- `-v, --verbose` (notazione doppio trattino) per produrre o meno come output le statistiche di elaborazione
    

Il file di input, ovvero il file contenente il codice C da processare, è un parametro obbligatorio e può essere passato come argomento dell'opzione `-i` o `--in`. Ad esempio 

    myPreCompiler nome_file_input.c

in questo caso il file `nome_file_input.c` verrà processato dal programma e l'output prodotto su stdout (vedi sezione Output)

    myPreCompiler -i nome_file_input.c 
    myPreCompiler --in=nome_file_input.c

in questo caso il file `nome_file_input.c` verra' processato dal programma e potra' essere utilizzata l'opzione `-o`, `--out` per specificare il file di output (vedi sezione Output).

### **Output**
Devono essere previste due modalità di output.

Nella prima, viene specificato come parametro di input del programma il nome del file di output quale argomento dell'opzione `-o, --out`. 

Nella seconda, se l'opzione `-o` non viene usata, il risultato del processamento del file di input viene inviato allo stdout.

#### Esempi di esecuzione:

    myPreCompiler -i nome_file_input.c -o nome_file_output

al termine del programma, il file `nome_file_output` conterra' il codice processato.

    myPreCompiler -i nome_file_input.c
    myPreCompiler nome_file_input.c

prima di terminare il programma invia su stdout il risultato del processamento

    myPreCompiler -i nome_file_input.c > nome_file_output

prima di terminare il programma invia sullo stdout il risultato del processamento che viene ridirezionato nel file `nome_file_output`.

Il programma myPreCompiler deve altresì poter restituire come risultato, sullo standard output, le seguenti statistiche di elaborazione:
- numero di variabili controllate
- numero di errori rilevati
- per ogni errore rilevato, nome del file in cui e' stato rilevato e numero di riga nel file.
- numero di righe di commento eliminate
- numero di file inclusi
- per il file di input, la dimensione in byte ed il numero di righe (pre-processamento)
- per ogni file incluso, dimensione in byte e numero di righe (pre-processamento)
- numero di righe e dimensione dell'output

L'output sopra riportato deve poter essere abilitato/disabilitato mediante l'opzione `-v`, `--verbose`. Ad esempio

    myPreCompiler -v -c nome_file_input.c -o nome_file_output

restituisce sullo standard output le statistiche sopra menzionate, mentre 

    myPreCompiler -c nome_file_input.c -o nome_file_output

non produce sullo standard output le statistiche sopra menzionate.

> (Opzionale: Restituire le statistiche di elaborazione sullo standard error invece che sullo standard output in modo da poter ridirezionare il file processato e le statistiche di elaborazione su due file diversi quando si seleziona la seconda modalità di output)

### Errori
L'applicazione sviluppata deve gestire i seguenti errori:

- errore nei parametri di input, opzioni e relativi argomenti specificate da linea di comando
- errore di apertura dei file di input o specificati come argomento dell'include
- errore di chiusura file
- errore di lettura da file - ad esempio file vuoto o corrotto
- errore di scrittura su file

### Struttura del programma e uso della memoria
- Il programma non deve essere monolotico ma composto da un main e varie funzioni che sviluppano le funzinalita' principali e funzionalita' di supporto.
- Il programma deve essere organizzato in almeno tre file: un file contenente il main, almeno un file contenente le funzioni, ed almeno un header file.
- Nella scelta delle strutture dati deve essere privilegiata l'allocazione dinamica della memoria.

In sede di esame il codice deve essere compilato da linea di comando usando il gcc.

### Testing
Deve essere provato il funzionamento del programma attraverso: 
- un pool di file fornito dal docente (vedi file allegati - ne seguiranno altri)
- un caso di test sviluppato dai componenti del gruppo
