# ourPreCompiler

`myPreCompiler` is a simple **C pre-compiler tool**. Given a C source file, it performs the following steps:

1. Resolves `#include` directives (includes the content of the files that are specified in the `#include` directives)
3. Checks whether declared variable names are valid identifiers (e.g., rejects names like `x-ray`, `two&four`, `5brothers`).
4. Removes all comments.
5. Produces an output file containing the modified code, expanded with the includes, and without comments.
6. Generates processing statistics specified in the "Statistics" section.

---

## Assumptions

* Included files (`#include`) are located in the **current working directory (CWD)**.
* The input file contains only the `main` function block, and there are no other functions.
* All local variables are declared at the beginning of `main` in contiguous lines.
* Global variables are declared before `main` in contiguous lines.
* Data types used in variable declarations are correct.

---

## Specifications

### Input

The program accepts **three command-line options**:

* `-i, --in` : input file (mandatory)
* `-o, --out` : output file 
* `-v, --verbose` : enable/disable statistics output

Examples:

```bash
myPreCompiler input_file.c
myPreCompiler -i input_file.c
myPreCompiler --in=input_file.c
```

If `-o` is not provided, the processed code is written to **stdout**.

---

### Output

There are two output modes:

1. **To file**: If `-o / --out` is specified, the processed result is written to the file.

   ```bash
   myPreCompiler -i input_file.c -o output_file
   ```

2. **To stdout**: If `-o` is not specified, the processed result is printed.

   ```bash
   myPreCompiler -i input_file.c
   myPreCompiler input_file.c
   ```

   Output sent to stdout can be redirected to a file like one normally would:
   ```
   myPreCompiler -i input_file.c > output_file
   ```

---

### Statistics

When the `-v, --verbose` option is enabled, the program outputs processing statistics, including:

* Number of variables checked
* Number of errors detected
* For each error: file name and line number
* Number of comment lines removed
* Number of files included
* For the input file: size in bytes and number of lines (pre-processing)
* For each included file: size in bytes and number of lines (pre-processing)
* Final output size in bytes and number of lines

Example:

```bash
myPreCompiler -v -i input_file.c -o output_file
```

> **Optional:** We've written the statistics to `stderr` instead of `stdout` to allow redirection of code and logs separately.

---

### Error Handling

The program handles the following errors:

* Invalid command-line arguments or options
* Failed to open input or included files
* File closing errors
* File reading errors (e.g., empty or corrupted file)
* File writing errors

---

### Program Structure

* The program isn't monolithic. It's composed of:

  * `main.c` file,
  * a `variables.c` file, which handles variable analyses, with its own `variables.h` header file
  * a `comments.c` file, which handles comments, with its own `comments.h` header file
  * an `arrays` file, where we defined a simple dynamic array
  * a `globals.c` file, with our global variables, used for the statistics

* The use of **dynamic memory allocation** was prioritised in the program

---

### Testing

The program can be tested with:

* A pool of files provided by the professor (test1 and test2)
* Some custom tests we created to cover more peculiar cases (test3)

---

## Usage Summary

```bash
# Process file and write to stdout
myPreCompiler input.c  

# Process file and save output
myPreCompiler -i input.c -o output.c  

# Process file and show statistics
myPreCompiler -v -i input.c  
```
