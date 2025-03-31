#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include "constants.h"
#include "parsetools.h"
#include "tokenizer.h"

void execute_command(int word_count, char** words) {
    if(word_count <= 0) {
        return; //no argument to execute
    }
    /*
     Purpose of below code: split the words array into null terminated commands
     Ex: input is ls -l | grep foo | wc -l
     words[0] = "ls"
     words[1] = "-l"
     words[2] = "|"
     words[3] = "grep"
     words[4] = "foo"
     words[5] = "|"
     words[6] = "wc"
     words[7] = "-l"

     commands[0][0] = "ls"
     commands[0][1] = "-l"
     commands[0][2] = NULL

     commands[1][0] = "grep"
     commands[1][1] = "foo"
     commands[1][2] = NULL

     commands[2][0] = "wc"
     commands[2][1] = "-l"
     commands[2][2] = NULL

     This allows us to use execvp, which requires a null terminated array
    */
    // First, find pipe locations and split the words array
    char** commands[MAX_PIPES + 1]; // Array of command arrays
    int cmd_count = 0;              // Number of commands
    int start_index = 0;            // Start index of current command

// Allocate memory for first command
    commands[cmd_count] = malloc(sizeof(char*) * MAX_LINE_WORDS);
    int cmd_arg_count = 0;

// Iterate through words
    for (int i = 0; i < word_count; i++) {
        if (strcmp(words[i], "|") == 0) {
            // End current command with NULL
            commands[cmd_count][cmd_arg_count] = NULL;
            cmd_count++;

            // Start new command
            commands[cmd_count] = malloc(sizeof(char*) * MAX_LINE_WORDS);
            cmd_arg_count = 0;
        } else {
            // Add word to current command
            commands[cmd_count][cmd_arg_count++] = words[i];
        }
    }

    // NULL-terminate the last command
    commands[cmd_count][cmd_arg_count] = NULL;
    cmd_count++; // Total number of commands

}

int main() {
    char input_line[MAX_LINE_CHARS];
    char* words[MAX_LINE_WORDS];
    int word_count;

    while (1) {
        // Display prompt
        printf("> ");

        // Get input
        if (fgets(input_line, MAX_LINE_CHARS, stdin) == NULL) {
            break; // Exit on EOF
        }

        // Split the input line into words
        word_count = split_cmd_line(input_line, words);

        if (word_count < MAX_LINE_WORDS) {
            words[word_count] = NULL;
        }

        // Execute the command
        execute_command(word_count, words);
    }

    return 0;
}


