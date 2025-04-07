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
#include "command.h"

enum state{
    executable,
    argument,
    left_redirection,
    right_redirection,
    append_redirection,
    string,
    pipe_state
};

struct Token{
    char* word;
    enum state s;
};

// Function to free the tokens array
void FreeTokens(struct Token* tokens, int tokenCount) {
    if (tokens == NULL) {
        return; // Nothing to free
    }

    for (int i = 0; i < tokenCount; i++) {
        if (tokens[i].word != NULL) {
            free(tokens[i].word); // Free each token's word
            tokens[i].word = NULL; // Reset pointer to avoid dangling references
        }
    }

    free(tokens); // Free the tokens array
    tokens = NULL; // Reset pointer to avoid dangling references
}

// Function to create a new command structure
struct Command* CreateCommand() {
    struct Command* cmd = malloc(sizeof(struct Command));
    cmd->argv = malloc(sizeof(char*) * MAX_LINE_WORDS);
    cmd->argc = 0;
    cmd->input_file = NULL;
    cmd->output_file = NULL;
    cmd->append = 0;
    cmd->next = NULL;
    return cmd;
}

// Function to free a pipeline of commands
void FreePipeline(struct Command* head) {
    while (head != NULL) {
        struct Command* next = head->next;
        FreeCommand(head);
        head = next;
    }
}

// Function to convert tokens into a pipeline of commands
struct Command* TokensToPipeline(struct Token* tokens, int tokenCount) {
    if (tokens == NULL || tokenCount == 0) {
        return NULL;
    }

    struct Command* head = NULL;
    struct Command* current = NULL;
    int start = 0;

    for (int i = 0; i <= tokenCount; i++) {
        // If we hit a pipe or the end of tokens, create a new command
        if (i == tokenCount || tokens[i].s == pipe_state) {
            struct Command* cmd = CreateCommand();
            
            // Process tokens for this command
            for (int j = start; j < i; j++) {
                switch (tokens[j].s) {
                    case left_redirection:
                        if (j + 1 < i) {
                            cmd->input_file = strdup(tokens[j + 1].word);
                            j++; // Skip the filename
                        }
                        break;
                    case right_redirection:
                        if (j + 1 < i) {
                            cmd->output_file = strdup(tokens[j + 1].word);
                            j++; // Skip the filename
                        }
                        break;
                    case append_redirection:
                        if (j + 1 < i) {
                            cmd->output_file = strdup(tokens[j + 1].word);
                            cmd->append = 1;
                            j++; // Skip the filename
                        }
                        break;
                    default:
                        cmd->argv[cmd->argc++] = strdup(tokens[j].word);
                        break;
                }
            }
            cmd->argv[cmd->argc] = NULL; // NULL terminate the argument array

            // Add to pipeline
            if (head == NULL) {
                head = cmd;
                current = cmd;
            } else {
                current->next = cmd;
                current = cmd;
            }

            start = i + 1; // Skip the pipe token
        }
    }

    return head;
}

// Function that begins tokenization by going character by character
// and determining the state of the character
struct Token* TokenizeTokens(int* tokenCount) {
    char d; // character read
    enum state s = executable; // current state
    char* currentWord = (char*)malloc(sizeof(char) * 100); // current word
    struct Token* tokens = (struct Token*)malloc(sizeof(struct Token) * 32); // array of tokens
    int tokenIndex = 0; // index of current token
    int currentWordIndex = 0;

    while((d = getchar()) != EOF) { // while not end of file
        if(d == '\n'){ // if character is newline
            if(currentWordIndex > 0){
                tokens[tokenIndex].word = strdup(currentWord);
                tokens[tokenIndex].s = s;
                tokenIndex++;
                currentWord[0] = '\0';
                currentWordIndex = 0;
            }
            *tokenCount = tokenIndex;
            free(currentWord);
            return tokens;
        }

        if(d == ' ' && s != string){ // if character is whitespace
            if (currentWordIndex > 0) { // Only add non-empty words
                tokens[tokenIndex].word = strdup(currentWord);
                tokens[tokenIndex].s = s;
                tokenIndex++;
                currentWord[0] = '\0';
                currentWordIndex = 0;

                if(s == executable){
                    s = argument; // change state to argument
                }
                if((s == left_redirection) || (s == right_redirection) || (s == append_redirection)){
                    s = argument; // change state to argument
                }
                if(s == pipe_state){
                    s = executable; // change state to executable
                }
            }
            continue; // Skip processing the space
        }
        else if(d == ' ' && s == string){
            currentWord[currentWordIndex++] = d; // add character to current word
            currentWord[currentWordIndex] = '\0'; // null terminate current word
        }

        if(s == string && d == '\"'){ // if current state is string and character is double quote
            s = argument; // change state to argument
            continue; // Skip adding the quote to the word
        }

        if(s == string){ // if current state is string
            if(d == '\\'){
                d = getchar(); // get next character
                currentWord[currentWordIndex++] = d; // add character to current word
                currentWord[currentWordIndex] = '\0'; // null terminate current word
            } else {
                currentWord[currentWordIndex++] = d; // add character to current word
                currentWord[currentWordIndex] = '\0'; // null terminate current word
            }
            continue; // skip to next character
        }
        
        else if(d == '\"'){
            s = string; // change state to string
            continue; // Skip adding the quote to the word
        }

        if(s == executable){ // if current state is whitespace
            currentWord[currentWordIndex++] = d; // add character to current word
            currentWord[currentWordIndex] = '\0'; // null terminate current word
        }

        else if((d == '<') && (s != string)){ // if character is redirection
            s = left_redirection; // change state to redirection
        }

        else if((d == '>') && (s != string)){
            s = right_redirection; // change state to redirection
            currentWord[currentWordIndex++] = d; // add character to current word
            currentWord[currentWordIndex] = '\0'; // null terminate current word
            d = getchar(); // get next character
            if(d == '>'){
                currentWord[currentWordIndex++] = d; // add character to current word
                currentWord[currentWordIndex] = '\0'; // null terminate current word
                s = append_redirection; // change state to redirection
            }
            else{
                ungetc(d, stdin); // put character back into input stream
            }
            continue;
        }

        else if((d == '|') && (s != string)){
            currentWord[currentWordIndex++] = d; // add character to current word
            currentWord[currentWordIndex] = '\0'; // null terminate current word
            s = pipe_state; // change state to pipe
        }

        else if(s == argument && (d != '\"')){ // if current state is argument
            currentWord[currentWordIndex++] = d; // add character to current word
            currentWord[currentWordIndex] = '\0'; // null terminate current word
        }
        
        if(s == right_redirection || s == left_redirection || s == append_redirection){ // if current state is redirection
            currentWord[currentWordIndex++] = d; // add character to current word
            currentWord[currentWordIndex] = '\0'; // null terminate current word
        }
    }
    free(currentWord);
    *tokenCount = tokenIndex;
    return tokens;
}

