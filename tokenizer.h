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

// Function that begins tokenization by going character by character
// and determining the state of the character
// Returns an array of tokens
void* TokenizeTokens(){

char d; // character read
enum state s = executable; // current state
char* currentWord = (char*)malloc(sizeof(char) * 32); // current word
struct Token* tokens = (struct Token*)malloc(sizeof(struct Token) * 32); // array of tokens
int tokenIndex = 0; // index of current token

while((d = getchar()) != EOF){ // while not end of file

    if(d == '\n'){ // if character is newline
        //execute command
        //free the array of tokens
        //get next command
        //reset token index
        //reset current word
        //reset state to executable
    }

    if(s == string && d == '\"'){ // if current state is string and character is double quote
        s = argument; // change state to argument
    }

    if(s == string){ // if current state is string
        currentWord = currentWord + d; // add character to current word
    }
    
    else if(d == '\"'){
        s = string; // change state to string
    }

    if(s == executable){ // if current state is whitespace
        currentWord = currentWord + d; // add character to current word
    
    }

    else if(d == '<'){ // if character is redirection
        s = left_redirection; // change state to redirection
    }

    else if(d == '>'){
        s = right_redirection; // change state to redirection
        d = getchar(); // get next character
        if(d == '>'){
            currentWord = currentWord + d; // add character to current word
            s = append_redirection; // change state to redirection
        }
        else{
            ungetc(d, stdin); // put character back into input stream
        }
    }

    else if(d == '|'){
        currentWord = currentWord + d; // add character to current word
        s = pipe_state; // change state to pipe
    }

    else if(s == argument && (d != '\"')){ // if current state is argument
        currentWord = currentWord + d; // add character to current word
    }
    
    if(s == right_redirection || s == left_redirection || s == append_redirection){ // if current state is redirection
        currentWord = currentWord + d; // add character to current word 
    }
    else if(d == ' ' && s != string){ // if character is whitespace
        tokens[tokenIndex].word = currentWord; // add current word to token array
        tokens[tokenIndex].s = s; // add state to token array
        tokenIndex++; // increment token index
        currentWord = (char*)malloc(sizeof(char) * 32); // reset current word

        if(s == executable){
            s = argument; // change state to argument
        }
        if((s == left_redirection) || (s == right_redirection) || (s == append_redirection)){
            s = argument; // change state to argument
        }
        if(s == pipe_state){
            s = executable; // change state to exacutable
        }
    }
    else if(d == ' ' && s == string){
        currentWord = currentWord + d; // add character to current word
    }
}
free(currentWord); // Free the last allocated currentWordcated currentWord

//loops that print out the tokens
for(int i = 0; i < tokenIndex; i++){
    printf("%s\n", tokens[i].word);
}
return NULL; // Since the function is void, you don't need to return anything.
}







// Function to free the tokens array
void FreeTokens(struct Token* tokens, int tokenCount) {
    for (int i = 0; i < tokenCount; i++) {
        free(tokens[i].word); // Free each token's word
    }
    free(tokens); // Free the tokens array
}