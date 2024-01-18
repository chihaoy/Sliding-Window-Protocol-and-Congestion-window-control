#include "host.h"
#include <assert.h>
#include "switch.h"

void handle_incoming_frames(Host* host) {
    // TODO: Suggested steps for handling incoming frames
    //    1) Dequeue the Frame from the host->incoming_frames_head
    //    2) Compute CRC of incoming frame to know whether it is corrupted
    //    3) If frame is corrupted, drop it and move on.
    //    4) Implement logic to check if the expected frame has come
    //    5) Implement logic to combine payload received from all frames belonging to a message
    //       and print the final message when all frames belonging to a message have been received.
    //    6) Implement the cumulative acknowledgement part of the sliding window protocol
    //    7) Append acknowledgement frames to the outgoing_frames_head queue
    int incoming_frames_length = ll_get_length(host->incoming_frames_head);
    //print income_frames_length to stederr

    //printf("incoming_frames_length: %d\n", incoming_frames_length);
    if (incoming_frames_length > 0) {
        char temp[incoming_frames_length + 1][59];
        // Pop a node off the front of the link list and update the count
        int t = incoming_frames_length;
        while (incoming_frames_length > 0){
            LLnode* ll_inmsg_node = ll_pop_node(&host->incoming_frames_head);
            incoming_frames_length = ll_get_length(host->incoming_frames_head);
        //print incoming_frames_length to stederr

        
            Frame* inframe = ll_inmsg_node->value; 
            
            strcpy(temp[incoming_frames_length], inframe->data); // Copy inframe->data into temp
            
            
            free(inframe);
        }
        char combinedString[(incoming_frames_length + 1) * 59];
        for (int i = t - 1; i >= 0;i--){
            strcat(combinedString, temp[i]);
        }
        

    // Print the result
        printf("<RECV_%d>:[%s]\n", host->id, combinedString);
        
    }

}

void run_receivers() {
    int recv_order[glb_num_hosts]; 
    get_rand_seq(glb_num_hosts, recv_order); 

    for (int i = 0; i < glb_num_hosts; i++) {
        int recv_id = recv_order[i]; 
        handle_incoming_frames(&glb_hosts_array[recv_id]); 
    }
}