#include "input.h"

#include <assert.h>

//*********************************************************************
// NOTE: We will overwrite this file, so whatever changes you put here
//      WILL NOT persist
//*********************************************************************

/* getline implementation is copied from glibc. */

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t) -1)
#endif
#ifndef SSIZE_MAX
#define SSIZE_MAX ((ssize_t)(SIZE_MAX / 2))
#endif

ssize_t getline(char** lineptr, size_t* n, FILE* fp) {
    ssize_t result;
    size_t cur_len = 0;

    if (lineptr == NULL || n == NULL || fp == NULL) {
        return -1;
    }

    if (*lineptr == NULL || *n == 0) {
        *n = 120;
        *lineptr = malloc(*n);
        if (*lineptr == NULL) {
            result = -1;
            goto end;
        }
    }

    for (;;) {
        int i;

        i = getc(fp);
        if (i == EOF) {
            result = -1;
            break;
        }

        /* Make enough space for len+1 (for final NUL) bytes.  */
        if (cur_len + 1 >= *n) {
            size_t needed_max =
                SSIZE_MAX < SIZE_MAX ? (size_t) SSIZE_MAX + 1 : SIZE_MAX;
            size_t needed = 2 * *n + 1; /* Be generous. */
            char* new_lineptr;

            if (needed_max < needed)
                needed = needed_max;
            if (cur_len + 1 >= needed) {
                result = -1;
                goto end;
            }

            new_lineptr = realloc(*lineptr, needed);
            if (new_lineptr == NULL) {
                result = -1;
                goto end;
            }

            *lineptr = new_lineptr;
            *n = needed;
        }

        (*lineptr)[cur_len] = i;
        cur_len++;

        if (i == '\n')
            break;
    }
    (*lineptr)[cur_len] = '\0';
    result = cur_len ? (ssize_t) cur_len : result;

end:
    return result;
}

void init_reader() {
    // Zero out fd_sets
    FD_ZERO(&glb_io_reader->read_fds);
    FD_ZERO(&glb_io_reader->master_fds);

    // Add standard input to the fd_set
    FD_SET(STDIN_FILENO, &glb_io_reader->master_fds);
    glb_io_reader->fd_max = STDIN_FILENO;
    glb_io_reader->timeout.tv_sec = 0; 
    glb_io_reader->timeout.tv_usec = 0;
}

int check_input() {

    int sender_id;
    int receiver_id;
    int sscanf_res;
    size_t input_buffer_size = (size_t) DEFAULT_INPUT_BUFFER_SIZE;
    char* input_buffer;
    char* input_message;
    int input_bytes_read;
    char input_command[MAX_COMMAND_LENGTH];

    // Copy the master set to avoid permanently altering the master set
    glb_io_reader->read_fds = glb_io_reader->master_fds;

    // User inputted a string, read in input and prepare to send
    while (select(glb_io_reader->fd_max + 1, &glb_io_reader->read_fds, NULL, NULL, &glb_io_reader->timeout) && FD_ISSET(STDIN_FILENO, &glb_io_reader->read_fds)) {
        input_buffer = malloc(input_buffer_size * sizeof(char));
        assert(input_buffer);

        // NULL set the entire input buffer
        memset(input_buffer, 0, input_buffer_size * sizeof(char));

        // Read in the command line into the input buffer
        input_bytes_read =
            getline(&input_buffer, &input_buffer_size, stdin);
        
        // If EOF is reached, getline returns -1
        if (input_bytes_read == -1){
            fprintf(stderr, "End Of File Reached\n");
            free(input_buffer);
            return -1; 
        }
        
        // Zero out the readin buffers for the command
        memset(input_command, 0, MAX_COMMAND_LENGTH * sizeof(char));

        // Zero out the memory for the message to communicate
        input_message = malloc((input_bytes_read + 1) * sizeof(char));
        assert(input_message);
        memset(input_message, 0, (input_bytes_read + 1) * sizeof(char));

        // Scan the input for the arguments
        sscanf_res = sscanf(input_buffer, "%s %d %d %[^\n]", input_command,
                            &sender_id, &receiver_id, input_message);

        // Number of parsed objects is less than expected
        if (sscanf_res < 4) {
            if (strcmp(input_command, "exit") == 0) {
                free(input_message);
                free(input_buffer);
                return 1;
            } else {
                fprintf(stderr, "Command is ill-formatted\n");
            }
        } else {
            if (strcmp(input_command, "msg") == 0) {
                // Check to ensure that the sender and receiver ids are in
                // the right range
                if (sender_id >= glb_num_hosts ||
                    sender_id < 0) {
                    
                    fprintf(stderr, "Sender id is invalid\n");
                }
                if (receiver_id >= glb_num_hosts ||
                    receiver_id < 0) {
                    //printf("receiver_id: %d\n", receiver_id);
                    fprintf(stderr, "Receiver id is invalid\n");
                }

                // Only add if valid
                if (sender_id < glb_num_hosts &&
                    receiver_id < glb_num_hosts &&
                    sender_id >= 0 && receiver_id >= 0) {
                    // Add the message to the receive buffer for the
                    // appropriate thread

                    Cmd* outgoing_cmd = malloc(sizeof(Cmd));
                    char* outgoing_msg =
                        malloc(sizeof(char) * (strlen(input_message) + 1));
                
                    // Copy out the input message into the outgoing command
                    // object
                    strcpy(outgoing_msg, input_message);

                    outgoing_cmd->src_id = sender_id;
                    outgoing_cmd->dst_id = receiver_id;
                    outgoing_cmd->message = outgoing_msg;

                    // Add it to the appropriate input buffer
                    Host* sender = &glb_hosts_array[sender_id];
                    // Lock the buffer, add to the input list, and signal
                    // the thread
                    ll_append_node(&sender->input_cmdlist_head,
                                    outgoing_cmd);
                }
            } else {
                fprintf(stderr, "Unknown command:%s\n", input_buffer);
            }
        }

        // Lastly, free the input_buffer and the input_message
        free(input_buffer);
        free(input_message);
    }

    return 0; 
}
