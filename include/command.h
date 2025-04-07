#ifndef COMMAND_H
#define COMMAND_H

struct Command {
    char** argv;        // Array of arguments (NULL-terminated)
    int argc;           // Number of arguments
    char* input_file;   // Input redirection file (NULL if none)
    char* output_file;  // Output redirection file (NULL if none)
    int append;         // 1 if output should be appended, 0 otherwise
    struct Command* next; // Next command in pipeline (NULL if none)
};

// Function to free a single command
void FreeCommand(struct Command* cmd) {
    if (cmd == NULL) return;

    // Free the argument vector
    if (cmd->argv != NULL) {
        for (int i = 0; cmd->argv[i] != NULL; i++) {
            free(cmd->argv[i]);
        }
        free(cmd->argv);
    }

    // Free input/output file strings if they exist
    if (cmd->input_file) free(cmd->input_file);
    if (cmd->output_file) free(cmd->output_file);

    // Finally free the command structure itself
    free(cmd);
}

#endif 