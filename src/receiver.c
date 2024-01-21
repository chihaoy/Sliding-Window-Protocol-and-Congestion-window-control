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
    int t = incoming_frames_length;
    //printf("incoming_frames_length: %d\n", incoming_frames_length);
    char temp[incoming_frames_length + 1][59];
    while (incoming_frames_length > 0) {
        // Pop a node off the front of the link list and update the count
        //print incoming_frames sequence number to stderr
        
        LLnode* ll_inmsg_node = ll_pop_node(&host->incoming_frames_head);
        incoming_frames_length = ll_get_length(host->incoming_frames_head);
        //print incoming_frames_length to stederr
        Frame* inframe = ll_inmsg_node->value; 
        //print inframe->seq_num
        printf("inframe->seq_num in receiver.c: %d\n", inframe->seq_num);
        //print inframe-> seq
        strcpy(temp[incoming_frames_length], inframe->data); // Copy inframe->data into temp
        //print inframe->data
        //print temp[incoming_frames_length]
        //printf("temp[%d]: %s\n", incoming_frames_length, temp[incoming_frames_length]);
        //free(inframe);
        if (incoming_frames_length == 0){
            char combinedString[(t + 1) * 59];
            memset(combinedString, 0, sizeof(combinedString));//so that no garbage character is present
            for (int i = t - 1; i >= 0;i--){
                
                strcat(combinedString, temp[i]);
            }
            Frame* outgoing_frame = (Frame*)malloc(sizeof(Frame));
            outgoing_frame->src_id = inframe->dst_id;
            outgoing_frame->dst_id = inframe->src_id;
            outgoing_frame->is_ack = 1;
            outgoing_frame->seq_num = inframe->seq_num;
            //print outgoing_frame->src_id and outgoing_frame->dst_id to stderr
            ll_append_node(&host->outgoing_frames_head, outgoing_frame);
            printf("<RECV_%d>:[%s]\n", host->id, combinedString);
        
        }
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