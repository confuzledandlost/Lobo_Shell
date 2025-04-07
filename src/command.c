#include <stdlib.h>
#include "command.h"

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