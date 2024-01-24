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
    //print incoming_frames_length to stederr
    //print income_frames_length to stederr
    int t = incoming_frames_length;
    //printf("incoming_frames_length: %d\n", incoming_frames_length);
    //char temp[incoming_frames_length + 1][57];
    //memset(temp, 0, sizeof(temp));
    while (incoming_frames_length > 0) {
        //printf("incoming_frames_length in recvewfewfewfeweiver.c: %d\n", incoming_frames_length);
        // Pop a node off the front of the link list and update the count
        //print incoming_frames sequence number to stderr
        //print incoming_frames_length
        LLnode* ll_inmsg_node = ll_pop_node(&host->incoming_frames_head);
        incoming_frames_length = ll_get_length(host->incoming_frames_head);

        //print incoming_frames_length to stederr
        Frame* inframe = ll_inmsg_node->value;
        //print(inframe->data) to stderr
        printf("infdasdasrame->data in receiver.c>>>: %s\n", inframe->data);
        if (inframe -> seq_num < host -> NFE ){
            
            return;
        }
        //printf("%s\n",inframe->data);
        //print inframe->seq_num
       // printf("inframe->seq_num in receiver.c: %d\n", inframe->seq_num);
        //print inframe-> seq
        printf("infdasdasrame->data in receiver.c: %s\n", inframe->data);
        strcat(host -> emptyCharArray, inframe->data); // Copy inframe->data into temp
        //print host -> emptyCharArray
      //  printf("host -> emptyCharArray: %s\n", host -> emptyCharArray);
        //print temp[incoming_frames_length]
        //print inframe->data
        //print temp[incoming_frames_length]
        //printf("temp[%d]: %s\n", incoming_frames_length, temp[incoming_frames_length]);
        //free(inframe);
        //print inframe -> remaining_msg_bytes
        //printf("inframe->remaining_msg_bytes in receiver.c: %d\n", inframe->remaining_msg_bytes);
       // 
       // if (inframe -> remaining_msg_bytes > 0){
            //host -> recvArray[1].sendQ[0].frame = inframe;
       // }
        //print host -> NFE
        printf("qwehost -> NFE: %d\n", host -> NFE);
        printf("qweinframe -> seq_num: %d\n", inframe -> seq_num);
        if (host -> NFE == inframe -> seq_num){
            //print inframe -> data
            
            Frame* outgoing_frame = (Frame*)malloc(sizeof(Frame));
            outgoing_frame->src_id = inframe->dst_id;
            outgoing_frame->dst_id = inframe->src_id;
            outgoing_frame->is_ack = 1;
            if (host -> NFE == 1 && inframe -> seq_num == 1){
                outgoing_frame->seq_num = 2;
            }
            else{
                outgoing_frame->seq_num = inframe->seq_num;
            }   
           // printf("infrfewfefame->seq_num in receiver.c: %d\n", inframe->seq_num);
            ll_append_node(&host->outgoing_frames_head, outgoing_frame);
            host -> NFE += 1;
        }
        if (inframe -> remaining_msg_bytes == 0){
            //print host -> recvArray[1].sendQ[0].frame -> data
            //printf("host -> recvArray[1].sendQ[0].frame -> data: %s\n", host -> recvArray[1].sendQ[0].frame -> data);
            //print temp[1]
            char combinedString[(t + 1) * 57];
            memset(combinedString, 0, sizeof(combinedString));//so that no garbage character is present
            //strcat(combinedString, host -> recvArray[1].sendQ[0].frame -> data);
            strcat(combinedString, host -> emptyCharArray);
            
            //print outgoing_frame->src_id and outgoing_frame->dst_id to stderr
            printf("<RECV_%d>:[%s]\n", host->id, combinedString);
            //memset(temp, 0, sizeof(temp));
            host -> emptyCharArray[0] = '\0';
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