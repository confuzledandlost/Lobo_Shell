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
        printf("Freeing token %d: %s\n", i, tokens[i].word);
        if (tokens[i].word != NULL) {
            free(tokens[i].word); // Free each token's word
            tokens[i].word = NULL; // Reset pointer to avoid dangling references
        }
    }

    free(tokens); // Free the tokens array
    tokens = NULL; // Reset pointer to avoid dangling references
}


// Function that begins tokenization by going character by character
// and determining the state of the character
void* TokenizeTokens(){

char d; // character read
enum state s = executable; // current state
char* currentWord = (char*)malloc(sizeof(char) * 32); // current word
struct Token* tokens = (struct Token*)malloc(sizeof(struct Token) * 32); // array of tokens
int tokenIndex = 0; // index of current token
int currentWordIndex = 0;

while((d = getchar()) != EOF){ // while not end of file
    
    if(d == '\n'){ // if character is newline
        //check if the tokens array is empty
        if(currentWordIndex > 0){
            printf("Creating Token: '%s', State: %d\n", currentWord, s);
            tokens[tokenIndex].word = strdup(currentWord); // add current word to token array
            tokens[tokenIndex].s = s; // add state to token array
            tokenIndex++; // increment token index
            currentWord[0] = '\0'; // reset current word
            currentWordIndex = 0; // reset current word index
        }


        if(tokenIndex == 0){
            printf("No Tokens"); // skip empty line
        }
        else{
        for (int i = 0; i < tokenIndex; i++) {
            printf("%s\n", tokens[i].word); // Print token
            switch (tokens[i].s) {
                case executable: printf("Executable\n"); break;
                case argument: printf("Argument\n"); break;
                case left_redirection: printf("Left Redirection\n"); break;
                case right_redirection: printf("Right Redirection\n"); break;
                case append_redirection: printf("Append Redirection\n"); break;
                case pipe_state: printf("Pipe\n"); break;
                case string: printf("String\n"); break;
                default: printf("Unknown State\n"); break;
            }
        }
    }
        FreeTokens(tokens, tokenIndex); // free the tokens array
        tokens = NULL;
        tokenIndex = 0; // reset token index
        s = executable; // change state to executable
        tokens = realloc(tokens, sizeof(struct Token) * (tokenIndex + 32)); // reallocate memory for tokens array
        continue; // skip to next line


        //execute command
        //free the array of tokens
        //get next command
        //reset token index
        //reset current word
        //reset state to executable
    }

    if(d == ' ' && s != string){ // if character is whitespace
        if (currentWordIndex > 0) { // Only add non-empty words
            printf("Creating Token: '%s', State: %d\n", currentWord, s);
            tokens[tokenIndex].word = strdup(currentWord); // Add current word to token array
            tokens[tokenIndex].s = s; // Add state to token array
            tokenIndex++; // Increment token index
            currentWord[0] = '\0'; // Reset current word
            currentWordIndex = 0; // Reset current word index

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
    }

    if(s == string){ // if current state is string
        if(d == '\\'){
            d = getchar(); // get next character
            currentWord[currentWordIndex++] = d; // add character to current word
            currentWord[currentWordIndex] = '\0'; // null terminate current word
        }
        continue; // skip to next character

    }
    
    else if(d == '\"'){
        s = string; // change state to string
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
free(currentWord); // Free the last allocated currentWordcated currentWord

return NULL; // Since the function is void, you don't need to return anything.
}

