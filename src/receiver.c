#include "host.h"
#include <assert.h>
#include "switch.h"
#include <stdbool.h>

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
       // printf("incoming_frames_length in ewfewfewfeweiver.c: %d\n", incoming_frames_length);
        // Pop a node off the front of the link list and update the count
        //print incoming_frames sequence number to stderr
        //print incoming_frames_length
        LLnode* ll_inmsg_node = ll_pop_node(&host->incoming_frames_head);
        incoming_frames_length = ll_get_length(host->incoming_frames_head);

        //print incoming_frames_length to stederr
        Frame* inframe = ll_inmsg_node->value;
        //print inframe -> data
        //printf("<wowRECV_%d>:[%s]\n", host->id, inframe->data);
        char* temp = convert_frame_to_char(inframe);
        if (compute_crc8(temp) != 0){
            return;
        }
         //   printf("CRC8 is not 0\n");
            //print compute_crc8(inframe)
         //   printf("compute_crc8(inframe):%d\n",compute_crc8(temp));
            
        //print inframe->seq_num to stderr
       // printf("helloinframe -> data:%s\n",inframe->data);
        //inframe -> data[strlen(inframe -> data) - 1] = '\0';
       // printf("helloinframe -> seq_num:%d\n",inframe->seq_num);
      //  printf("host -> recArray[inframe -> dst_id].NFE:%d\n",host -> recvArray[inframe -> src_id].NFE);
       // printf("host -> recArray[inframe -> dst_id].NFE + glb_sysconfig.window_size - 1:%d\n",host -> recvArray[inframe -> src_id].NFE + glb_sysconfig.window_size - 1);
        uint8_t k1 = host -> recvArray[inframe -> src_id].NFE;
        if (inframe -> is_ack == 1){
            continue;
        }
        if (seq_num_diff(k1,inframe->seq_num) >= glb_sysconfig.window_size || seq_num_diff(k1,inframe->seq_num) < 0){
            //dasdsadwqdqwdw");
            //printf("HWLLO");
            Frame* outgoing_frame = (Frame*)malloc(sizeof(Frame));
            outgoing_frame->src_id = inframe->dst_id;
            outgoing_frame->dst_id = inframe->src_id;
            outgoing_frame->is_ack = 1;
            outgoing_frame->data[0] = '\0';
            outgoing_frame->len = 0;
            outgoing_frame->starting_index = 0;
            outgoing_frame->seq_num = 0;
            outgoing_frame->remaining_msg_bytes = 0;
            outgoing_frame->ack_num = (host -> recvArray[inframe -> src_id].NFE) - 1;
            outgoing_frame->crc = 0;
            char *outgoing_charbuf = convert_char_to_frame(outgoing_frame);
            outgoing_frame -> crc = compute_crc8(outgoing_charbuf);
            outgoing_charbuf = convert_frame_to_char(outgoing_frame);
            ll_append_node(&host->outgoing_frames_head, outgoing_charbuf);
            continue;
        }
        //printf("loveinframe -> data:%s\n",inframe -> data);
        //printf("loveinframe -> len:%d\n",inframe -> len);
        //printf("loveinframe -> starting_index:%d\n",inframe -> starting_index);
        Frame * cop = (Frame *) malloc (sizeof(Frame));
        memset(cop, 0, sizeof(Frame));
        memcpy(cop, inframe, sizeof(Frame));
        host -> recvArray[inframe -> src_id].receive_window[inframe -> seq_num % glb_sysconfig.window_size].frame = cop;
        //printf("host -> recvArray[inframe -> src_id].receive_window[inframe -> seq_num % glb_sysconfig.window_size].frame -> data:%s\n",host -> recvArray[inframe -> src_id].receive_window[inframe -> seq_num % glb_sysconfig.window_size].frame -> data);
        //printf("whathelloabcdinframe -> data:%s\n",host -> recvArray[inframe -> src_id].receive_window[inframe -> seq_num % glb_sysconfig.window_size].frame->data);
       // printf("helloabcdinframe -> data:%s\n",host -> recvArray[inframe -> seq_num % glb_sysconfig.window_size].frame->data);
        int pre_seq = -1;
        if (host -> recvArray[inframe -> src_id].NFE == inframe -> seq_num){
           // printf("Should have something here\n");
            //printf("host ->NFE in receiver.c:%d\n",host -> NFE);
            pre_seq = inframe -> seq_num - 1; 
            while (host -> recvArray[inframe -> src_id].receive_window[(host ->recvArray[inframe -> src_id].NFE) % glb_sysconfig.window_size].frame != NULL && pre_seq < host -> recvArray[inframe->src_id].receive_window[(host->recvArray[inframe -> src_id].NFE) % glb_sysconfig.window_size].frame -> seq_num){//still should check the case for window_size of 8
                //print inframe->data
                //print inframe->len
                //print inframe->starting_index
                Frame* temp = host -> recvArray[inframe -> src_id].receive_window[(host ->recvArray[inframe -> src_id].NFE) % glb_sysconfig.window_size].frame;
                memcpy(host -> recvArray[inframe -> src_id].emptyCharArray + temp->starting_index, temp -> data,temp -> len); // Copy inframe->data into temp
                host -> recvArray[inframe -> src_id].receive_window[(host ->recvArray[inframe -> src_id].NFE) % glb_sysconfig.window_size].frame = NULL;
                //print host -> emptyCharArray to stderr
                //printf("<nio_%d>:[%s]\n", host->id, host -> recvArray[inframe -> src_id].emptyCharArray);
                //print host -> emptyCharArray to stderr
                //printf("<RdwqdwqwdECV_%d>:[%s]\n", host->id, host -> recvArray[inframe -> src_id].emptyCharArray);
                //print host -> emptyCharArray to stderr
                //print host -> emptyCharArray to stderr
               // printf("<Rr32r32ECV_%d>:[%s]\n", host->id, host -> emptyCharArray);
                //printf("howareyouhost -> NFE in receiver.c:%d\n",host -> NFE);
               // printf("emptyCharArray:%s\n",host -> emptyCharArray);
                //printf("host -> receive_windo -> remain:%i\n",host -> receive_window[(host -> NFE) % glb_sysconfig.window_size].frame -> remaining_msg_bytes);
                //print inframe->remaining_msg_bytes to stderr
              //  printf("host -> emptyCharArray:%s\n",host -> emptyCharArray);
              //  printf("host -> recArray[inframe->dst_id].receive_window[(host -> recArray[inframe -> dst_id].NFE) glb_sysconfig.window_size].frame -> remaining_msg_bytes%d\n",host -> recvArray[inframe->src_id].receive_window[(host -> recvArray[inframe -> src_id].NFE) % glb_sysconfig.window_size].frame -> remaining_msg_bytes);
                if (temp -> remaining_msg_bytes == 0){
                    char combinedString[65535];
                    
                   // printf("YAY\n");
                    memset(combinedString, 0, sizeof(combinedString));//so that no garbage character is present
                    //set combinesString to '\0'
                    combinedString[0] = '\0';


                                        //strcat(combinedString, host -> recvArray[1].sendQ[0].frame -> data);
                    //strcat(combinedString, host -> emptyCharArray);
                    strcpy(combinedString, host -> recvArray[inframe -> src_id].emptyCharArray);
                    //print outgoing_frame->src_id and outgoing_frame->dst_id to stderr
                    //combinedString[strlen(combinedString)] = '\0';
                    printf("<RECV_%d>:[%s]\n", host->id, combinedString);

                    //memset(temp, 0, sizeof(temp));
                    host -> recvArray[inframe -> src_id].emptyCharArray[0] = '\0';
                }
             //   printf("HELLO\n");
                host -> recvArray[inframe -> src_id].NFE += 1;

                //printf("specialhost -> NFE:%d\n",host -> NFE);
                pre_seq = inframe -> seq_num;
            //    printf("BYE\n");
                //printf("window_size%s",host -> receive_window[(host -> NFE) % glb_sysconfig.window_size].frame -> data);
            }
            //print host -> emptyCharArray to stderr
          //  printf("I want to see thisfwqdwd!");
            Frame* outgoing_frame = (Frame*)malloc(sizeof(Frame));
            outgoing_frame->src_id = inframe->dst_id;
            outgoing_frame->dst_id = inframe->src_id;
            outgoing_frame->is_ack = 1;
            outgoing_frame->data[0] = '\0';
            outgoing_frame->len = 0;
            outgoing_frame->starting_index = 0;
            outgoing_frame->seq_num = 0;
            outgoing_frame->remaining_msg_bytes = 0;
            //Frame* outgoing_charbuf = convert_char_to_frame(host -> emptyCharArray);
            //printf("Current message of the host: %s\n", host -> emptyCharArray);
            //printf("hello\n");
           
            //print inframe -> src_id to stderr
           // printf("qsdoutgoing_frame->src_id:%d\n",outgoing_frame->src_id);
            //print inframe -> dst_id to stderr
           // printf("qsdoutgoing_frame->dst_id:%d\n",outgoing_frame->dst_id);
            outgoing_frame->ack_num = (host -> recvArray[inframe -> src_id].NFE) - 1;
            //print
            //print outgoing_frame->ack_num
            outgoing_frame->crc = 0;
            char* outgoing_charbuf = convert_char_to_frame(outgoing_frame);
            outgoing_frame -> crc = compute_crc8(outgoing_charbuf);
            outgoing_charbuf = convert_frame_to_char(outgoing_frame);
            ll_append_node(&host->outgoing_frames_head, outgoing_frame);


            //printf("outgoing_frame -> ack_num:%d\n",outgoing_frame->ack_num);
            //print length of outgoing_frames_head to stderr
           // printf("length of outgoing_frames_head:%d\n",ll_get_length(host->outgoing_frames_head));      
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