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
    exacutable,
    argument,
    redirection,
    pipe
};

struct Token{
    char* word;
    enum state s;
};



int main() {

    // Buffer for reading one line of input
    //char line[MAX_LINE_CHARS];
    // holds separated words based on whitespace
    //char* line_words[MAX_LINE_WORDS + 1];

    // Loop until user hits Ctrl-D (end of input)
    // or some other input error occurs
    //while( fgets(line, MAX_LINE_CHARS, stdin) ) {
        //int num_words = split_cmd_line(line, line_words);

        //for (int i=0; i < num_words; i++) {
            //printf("%s\n", line_words[i]);
        //}
    //}
    
    char d; // character read
    enum state s = exacutable; // current state
    char* currentWord = malloc(sizeof(char) * 32); // current word
    struct Token* tokens = malloc(sizeof(struct Token) * 32); // array of tokens
    int tokenIndex = 0; // index of current token

    while(d = getchar() != EOF){ // while not end of file
        if(s == exacutable){ // if current state is whitespace
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

            if(s == exacutable){
                s = argument; // change state to argument
            }
            if(s == argument){
                s = redirection; // change state to redirection
            }
            if(s == redirection){
                s = pipe; // change state to pipe
            }
            if(s == pipe){
                s = exacutable; // change state to exacutable
            }
        }
    }

    return 0;
}


