#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include "include/constants.h"
#include "include/parsetools.h"
#include "include/tokenizer.h"
#include "include/command.h"

// for file in tests/*.sh; do echo "🔸 Running: $file"; sh "$file" echo "-----------------------------"; done

// Function to execute a pipeline of commands
void execute_pipeline(struct Command* head) {
    if (head == NULL) {
        return;
    }

    int pipefd[2];
    int prev_pipe_read = -1;
    struct Command* current = head;

    while (current != NULL) {
        // Create pipe if there's another command after this one
        if (current->next != NULL) {
            if (pipe(pipefd) < 0) {
                perror("pipe");
                return;
            }
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return;
        } else if (pid == 0) { // Child process
            // Handle input redirection or pipe from previous command
            if (current->input_file != NULL) {
                int fd = open(current->input_file, O_RDONLY);
                if (fd < 0) {
                    perror("open input file");
                    exit(EXIT_FAILURE);
                }
                if (dup2(fd, STDIN_FILENO) < 0) {
                    perror("dup2 input");
                    exit(EXIT_FAILURE);
                }
                close(fd);
            } else if (prev_pipe_read != -1) {
                if (dup2(prev_pipe_read, STDIN_FILENO) < 0) {
                    perror("dup2 pipe input");
                    exit(EXIT_FAILURE);
                }
                close(prev_pipe_read);
            }

            // Handle output redirection or pipe to next command
            if (current->output_file != NULL) {
                int flags = O_WRONLY | O_CREAT;
                if (current->append) {
                    flags |= O_APPEND;
                } else {
                    flags |= O_TRUNC;
                }
                int fd = open(current->output_file, flags, 0666);
                if (fd < 0) {
                    perror("open output file");
                    exit(EXIT_FAILURE);
                }
                if (dup2(fd, STDOUT_FILENO) < 0) {
                    perror("dup2 output");
                    exit(EXIT_FAILURE);
                }
                close(fd);
            } else if (current->next != NULL) {
                if (dup2(pipefd[1], STDOUT_FILENO) < 0) {
                    perror("dup2 pipe output");
                    exit(EXIT_FAILURE);
                }
                close(pipefd[1]);
            }

            // Close unused pipe ends
            if (prev_pipe_read != -1) {
                close(prev_pipe_read);
            }
            if (current->next != NULL) {
                close(pipefd[0]);
            }

            // Execute the command
            if (execvp(current->argv[0], current->argv) < 0) {
                perror("execvp");
                exit(EXIT_FAILURE);
            }
        }

        // Parent process
        if (prev_pipe_read != -1) {
            close(prev_pipe_read);
        }
        if (current->next != NULL) {
            close(pipefd[1]);
            prev_pipe_read = pipefd[0];
        }

        current = current->next;
    }

    // Wait for all child processes
    while (wait(NULL) > 0);
}

int main() {
    int tokenCount;
    
    while (1) {
        // Get tokens from input
        struct Token* tokens = TokenizeTokens(&tokenCount);
        if (tokens->s == End_Of_File) {
            break; // Exit on EOF
        }

        // Convert tokens to pipeline of commands
        struct Command* pipeline = TokensToPipeline(tokens, tokenCount);
        
        // Execute the pipeline if valid
        if (pipeline != NULL) {
            execute_pipeline(pipeline);
            FreePipeline(pipeline);
        }

        // Clean up tokens
        FreeTokens(tokens, tokenCount);
    }

    return 0;
}


