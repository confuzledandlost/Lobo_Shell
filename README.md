# Lobo Shell

A custom shell implementation for CS-450 (Operating Systems) that supports basic shell functionality including command execution, piping, and I/O redirection.

## Features

- Command execution with arguments
- Pipe support (`|`) for command chaining
- Input redirection (`<`)
- Output redirection (`>`)
- Append redirection (`>>`)
- String handling with quotes
- Built-in command support

## Team Members

- Brandon Robinson
- Josh Lozano
- Sean Belingheri

## Building

To build the shell, simply run:

```bash
make
```

This will compile the source files and create the executable.

## Usage

Run the shell:

```bash
./lobo-shell
```

### Examples

1. Basic command execution:
```bash
ls -l
```

2. Piping commands:
```bash
ls | grep .txt
```

3. Input redirection:
```bash
cat < input.txt
```

4. Output redirection:
```bash
ls > output.txt
```

5. Append redirection:
```bash
echo "new line" >> output.txt
```

## Project Structure

- `src/` - Source code files
- `include/` - Header files
- `tests/` - Test scripts
- `main.c` - Main program entry point
- `makefile` - Build configuration

## Implementation Details

The shell is implemented in C and uses the following key components:
- Tokenizer for parsing input
- Command structure for representing commands and their arguments
- Pipeline execution for handling pipes
- File redirection handling for I/O operations

## License

This project is part of CS-450 coursework and is for educational purposes.
