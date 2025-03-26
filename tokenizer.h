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
    redirection,
    pipe
};

struct Token{
    char* word;
    enum state s;
};

// Function that begins tokenization
void * Tokenize(){

char d; // character read
enum state s = executable; // current state
char* currentWord = malloc(sizeof(char) * 32); // current word
struct Token* tokens = malloc(sizeof(struct Token) * 32); // array of tokens
int tokenIndex = 0; // index of current token

while(d = getchar() != EOF){ // while not end of file
    if(s == executable){ // if current state is whitespace
        currentWord = currentWord + d; // add character to current word
    
    }

    if(s == argument && ((d != '<') || (d != '>'))){ // if current state is argument
        currentWord = currentWord + d; // add character to current word
    }
    
    if(d == '<' || d == '>'){ // if character is redirection
        s = redirection; // change state to redirection
    }

    if(s == redirection){
        currentWord = currentWord + d; // add character to current word 
    }
    if(d == ' '){ // if character is whitespace
        tokens[tokenIndex].word = currentWord; // add current word to token array
        tokens[tokenIndex].s = s; // add state to token array
        tokenIndex++; // increment token index
        currentWord = malloc(sizeof(char) * 32); // reset current word

        if(s == executable){
            s = argument; // change state to argument
        }
        if(s == argument){
            s = redirection; // change state to redirection
        }
        if(s == redirection){
            s = pipe; // change state to pipe
        }
        if(s == pipe){
            s = executable; // change state to exacutable
        }
    }
}
}