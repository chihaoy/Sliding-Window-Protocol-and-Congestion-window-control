#include "host.h"
#include <assert.h>
#include "switch.h"

struct timeval* host_get_next_expiring_timeval(Host* host) {
    // TODO: You should fill in this function so that it returns the 
    // timeval when next timeout should occur
    // 
    // 1) Check your send_window for the timeouts of the frames. 
    // 2) Return the timeout of a single frame. 
    
    // HINT: It's not the frame with the furtherst/latest timeout. 
    //return the earliest timeout from the send_window
    
    struct timeval* next_timeout = NULL;
    for (int i = 0; i < glb_sysconfig.window_size; i++) {
        if (host->send_window[i].timeout != NULL) {
            //printf("hello");
            if (next_timeout == NULL) {
              //  printf("wahts");
                next_timeout = host->send_window[i].timeout;
            } else if (timeval_usecdiff(host->send_window[i].timeout, next_timeout) >= 0) {
              //  printf("well");
                next_timeout = host->send_window[i].timeout;
            }
        }
    }
    //print next_timeout
    if (next_timeout != NULL) {
        printf("next_timeout in sender.c: %ld\n", next_timeout->tv_usec + next_timeout->tv_sec * 1000000);
    }
    //printf("next_timeout in sender.c: %ld\n", next_timeout->tv_usec + next_timeout->tv_sec * 1000000);
    return next_timeout;
}

void handle_incoming_acks(Host* host, struct timeval curr_timeval) {
    //print "hello"
    // Num of acks received from each receiver
    uint8_t num_acks_received[glb_num_hosts]; 
    memset(num_acks_received, 0, glb_num_hosts); 

    // Num of duplicate acks received from each receiver this rtt
    uint8_t num_dup_acks_for_this_rtt[glb_num_hosts];     //PA1b
    memset(num_dup_acks_for_this_rtt, 0, glb_num_hosts); 

    // TODO: Suggested steps for handling incoming ACKs
    //    1) Dequeue the ACK frame from host->incoming_frames_head
    //    2) Compute CRC of the ack frame to know whether it is corrupted
    //    3) Check if the ack is valid i.e. within the window slot 
    //    4) Implement logic as per sliding window protocol to track ACK for what frame is expected,
    //       and what to do when ACK for expected frame is received
    //print host->window_size[0]->frame->data
        //printf("host->window_size[0]->frame->data in sender.c: %s\n", host->send_window[0].frame->data);
        

        
        //ll_destroy_node(host->send_window[0].frame);
        
    //prit tac->seq_num

    //print 
    while (ll_get_length(host->incoming_frames_head) != 0) {
        //print inframe->seq_num
        //print ll_get_length(host->incoming_frames_head)
        LLnode* ll_input_cmd_node = ll_pop_node(&host->incoming_frames_head);
        Frame* inframe = (Frame*) ll_input_cmd_node->value;
        //print inframe->src_id and inframe->dst_id
        
       // printf("inframe->seq_nfewfewfewum: %d\n", inframe->seq_num);
        //print inframe->is_ack
      //  printf("host -> LAR before if: %d\n", host -> LAR);
        //print inframe -> is_ack
        //printf("acdinframe -> is_ack: %d\n", inframe -> is_ack);
        uint8_t k = 0;
        if (inframe -> seq_num == 2){
            k = 2;
        }
        else if (inframe -> seq_num == 1){
            k = 1;
        }
        else{
            k = 0;
        }
        
        //print inframe ->seq_num
       printf("acdinframe -> seq_num: %d\n", inframe -> seq_num);

      //  printf("acdwe waht %d",k);
        while (k <= inframe->seq_num){
           // printf("we waht %d",k);
            if (inframe -> is_ack == 1){
                printf("host -> LAR: inside if%d\n", (host -> LAR) % glb_sysconfig.window_size);
                free(host->send_window[(host -> LAR) % glb_sysconfig.window_size].frame);
                host->send_window[(host -> LAR) % glb_sysconfig.window_size].frame = NULL;
                host -> LAR = host -> LAR + 1;
               
            //print (host -> LAR) % glb_sysconfig.window_size
            //printf("host -> LAR: %d\n", host -> LAR);
                
                //print host->send_window[(host->LAR) % glb_sysconfig.window_size].frame
                //printf("host->send_window[i].frame->data in sender.c: %s\n", host->send_window[1].frame -> data);
                
            }
            k= k + 1;
            //print k
          //  printf("k: %d\n", k);
        }
        //print inframe -> seq_num
       // printf("packet num %d\n", inframe -> seq_num);
        //print ost -> send_window[(host->LAR) - 1].frame
        //print host->id
        if (host -> LAR == inframe -> seq_num){
            //printf("AAA");
            break;
        }
    }
    
    
    //print inframe->src_id and inframe->dst_id
    
    if (host->id == glb_sysconfig.host_send_cc_id) {
        fprintf(cc_diagnostics,"%d,%d,%d,",host->round_trip_num, num_acks_received[glb_sysconfig.host_recv_cc_id], num_dup_acks_for_this_rtt[glb_sysconfig.host_recv_cc_id]); 
    }
}

void handle_input_cmds(Host* host, struct timeval curr_timeval) {
    // TODO: Suggested steps for handling input cmd
    //    1) Dequeue the Cmd from host->input_cmdlist_head
    //    2) Implement fragmentation if the message length is larger than FRAME_PAYLOAD_SIZE
    //    3) Set up the frame according to the protocol
    //    4) Append each frame to host->buffered_outframes_head

    int input_cmd_length = ll_get_length(host->input_cmdlist_head);
    while (input_cmd_length > 0) {
        // Pop a node off and update the input_cmd_length
        LLnode* ll_input_cmd_node = ll_pop_node(&host->input_cmdlist_head);
        input_cmd_length = ll_get_length(host->input_cmdlist_head);
        // Cast to Cmd type and free up the memory for the node
        
        Cmd* outgoing_cmd = (Cmd*) ll_input_cmd_node->value;
        free(ll_input_cmd_node);
        
        int msg_length = strlen(outgoing_cmd->message) + 1; // +1 to account for null terminator 
        //print msg_length
        
        if (msg_length > FRAME_PAYLOAD_SIZE) {
            // Do something about messages that exceed the frame size
            printf(
                "<SEND_%d>: sending messages of length greater than %d is not "
                "implemented\n",
                host->id, 57);
            
            
            //mimic the else part, but deal with storing first 64 bytes into buffer[0] and next 64 into buffer[1]
            //and so on

            int num_frames = ceil((double)msg_length / 57);
            //print num_frames
            for (int i = 0; i < num_frames; i++) {
                Frame* outgoing_frame = malloc(sizeof(Frame));
                assert(outgoing_frame);
                outgoing_frame->src_id = outgoing_cmd->src_id;
                outgoing_frame->dst_id = outgoing_cmd->dst_id;
                outgoing_frame->remaining_msg_bytes = max(msg_length - (i + 1) * 57,0);
                uint16_t numbytes = 0;
                if (outgoing_frame->remaining_msg_bytes > 0) {
                    numbytes = 57;
                } else {
                    numbytes = msg_length - i * 57;
                }
              //  printf("outgoing_frame->remaining_msg_bytes in handle_input_cmds: %d\n", outgoing_frame->remaining_msg_bytes);
                memcpy(outgoing_frame->data, outgoing_cmd->message + i * 57, numbytes);
                ll_append_node(&host->buffered_outframes_head, outgoing_frame);
                
                
            } 
           //print ll_get_length(host->buffered_outframes_head)
           
        } else {
            Frame* outgoing_frame = malloc(sizeof(Frame));
            assert(outgoing_frame);
            strcpy(outgoing_frame->data, outgoing_cmd->message);
            //print outgoing_frame->data
            outgoing_frame->remaining_msg_bytes = 0;
            outgoing_frame->src_id = outgoing_cmd->src_id;
            outgoing_frame->dst_id = outgoing_cmd->dst_id;
            // At this point, we don't need the outgoing_cmd
            free(outgoing_cmd->message);
            free(outgoing_cmd);
            
            
            ll_append_node(&host->buffered_outframes_head, outgoing_frame);
            
        }
    }
   // printf("host->buffered_outframes_head in handle_input_cmds: %d\n", ll_get_length(host->buffered_outframes_head));
}

void handle_timedout_frames(Host* host, struct timeval curr_timeval) {

    // TODO: Detect frames that have timed out
    // Check your send_window for the frames that have timed out and set send_window[i]->timeout = NULL
    // You will re-send the actual frames and set the timeout in handle_outgoing_frames()
    //print curr_timeval
    printf("curr_timeval in sender.c: %ld\n", curr_timeval.tv_usec + curr_timeval.tv_sec * 1000000);
    for (int i = 0; i < glb_sysconfig.window_size; i++) {
        if (host->send_window[i].timeout != NULL && timeval_usecdiff(&curr_timeval, host->send_window[i].timeout) <= 0) {
            printf("Here");
            //print i
          //  printf("%d", i);
            printf("there\n");
            host->send_window[i].timeout = NULL;

        }
    }
    
}

void handle_outgoing_frames(Host* host, struct timeval curr_timeval) {

    long additional_ts = 0; 

    if (timeval_usecdiff(&curr_timeval, host->latest_timeout) > 0) {
        memcpy(&curr_timeval, host->latest_timeout, sizeof(struct timeval)); 
    }

    //TODO: Send out the frames that have timed out(i.e. timeout = NULL)
    
    for (int i = 0; i < glb_sysconfig.window_size; i++) {
        if (host->send_window[i].timeout == NULL && host->send_window[i].frame != NULL){
            printf("BCGDDAA\n");
            printf("%d\n",i);
            printf("BCGDDAA\n");
         //   printf("awfeqfwqdwqhost->send_window[i].frame->data in sender.c: %d\n", host->send_window[i].frame -> dst_id);
         //   printf("awfeqfwqdwqhost->send_window[i].frame->data in sender.c: %d\n", host->send_window[i].frame -> src_id);
            //print send_window[i].frame->data
          //  printf("acderoutgoing_frame->data in sender.c: %s\n", host -> send_window[i].frame->data);
            //printf("host->send_window[i].frame->data in sender.c: %s\n", host->send_window[1].frame -> data);
            struct timeval* next_timeout = malloc(sizeof(struct timeval));
            memcpy(next_timeout, &curr_timeval, sizeof(struct timeval)); 
            timeval_usecplus(next_timeout, TIMEOUT_INTERVAL_USEC + additional_ts);
            Frame * cop = (Frame *) malloc (sizeof(Frame));
            memset(cop, 0, sizeof(Frame));
            host->send_window[i].timeout = next_timeout;//(this is what I add)
            memcpy(cop, host->send_window[i].frame, sizeof(Frame));
           // printf("resendhost->send_window[i].frame->data in sender.c: %s\n", host->send_window[i].frame->data);
            ll_append_node(&host->outgoing_frames_head, cop);
            additional_ts += 10000; //ADD ADDITIONAL 10ms
        }
    }
    
    //TODO: The code is incomplete and needs to be changed to have a correct behavior
    //Suggested steps: 
    //1) Within the for loop, check if the window is not full and there's space to send more frames 
    //2) If there is, pop from the buffered_outframes_head queue and fill your send_window_slot data structure with appropriate fields. 
    //3) Append the popped frame to the host->outgoing_frames_head
    
    //print ll_get_length(host->buffered_outframes_head)
    //print ll_get_length(host->buffered_outframes_head)
   // printf("host->buffered_outframes_head in handle_outgoing_frames: %d\n", ll_get_length(host->buffered_outframes_head));
    for (int i = 0; i < glb_sysconfig.window_size && ll_get_length(host->buffered_outframes_head) > 0; i++) {
        //printf("i: %d\n", i);
        if (host->send_window[i].frame == NULL) {
            //print i
            
            LLnode* ll_outframe_node = ll_pop_node(&host->buffered_outframes_head);
            
            Frame* outgoing_frame = ll_outframe_node->value; 
            
            //print outgoing_frame->data
            host->LFS = host->LFS + 1;
            outgoing_frame->seq_num = host->LFS;
            outgoing_frame -> is_ack = 0;
            char * outgoing_charbuf = convert_frame_to_char(outgoing_frame);
          //  printf("testing: %s\n", outgoing_frame -> data);
            Frame * cop = (Frame *) malloc (sizeof(Frame));
            memset(cop, 0, sizeof(Frame));
          //  printf("qwaaaerthost->send_window[i].frame->data in seawernder.c src_id: %d\n", outgoing_frame -> src_id);
         //   printf("qwaaaerthost->send_window[i].frame->data in seawernder.c dst_id: %d\n", outgoing_frame -> dst_id);
            memcpy(cop, outgoing_frame, sizeof(Frame));
         //   printf("qwerthost->send_window[i].frame->data in seawernder.c src_id: %d\n", cop -> src_id);
          //  printf("qwerthost->send_window[i].frame->data in seawernder.c dst_id: %d\n", cop -> dst_id);
            //print outgoing_frame->data
            printf("hellooutgoing_frame->data in sender.c: %s\n", outgoing_frame->data);
            host->send_window[i].frame = cop;
            ll_append_node(&host->outgoing_frames_head, outgoing_charbuf); 
            //print host->send_window[i].frame
           
            
             
        //    printf("I am allocating memort for %d:\n",i);
            


            //Set a timeout for this frame
            //NOTE: Each dataframe(not ack frame) that is appended to the 
            //host->outgoing_frames_head has to have a 10ms offset from 
            //the previous frame to enable selective retransmission mechanism. 
            //Already implemented below
            struct timeval* next_timeout = malloc(sizeof(struct timeval));
            memcpy(next_timeout, &curr_timeval, sizeof(struct timeval)); 
            timeval_usecplus(next_timeout, TIMEOUT_INTERVAL_USEC + additional_ts);
            host->send_window[i].timeout = next_timeout;//(this is what I add)
            //print host->send_window[i].timeout
            printf("host->send_window[i].frame->data in sender.c: %s\n", host->send_window[i].frame->data);
            printf("host->send_window[i].timeout in sender.c: %ld\n", host->send_window[i].timeout->tv_usec + host->send_window[i].timeout->tv_sec * 1000000);
            additional_ts += 10000; //ADD ADDITIONAL 10ms
            
            //print host->send_window[i].timeout
            free(ll_outframe_node);
        }
    }
   // printf("host->buffered_outframes_head in handle_outgoing_frames: %d\n", ll_get_length(host->buffered_outframes_head));
    memcpy(host->latest_timeout, &curr_timeval, sizeof(struct timeval)); 
    timeval_usecplus(host->latest_timeout, additional_ts);
    
    //NOTE:
    // Don't worry about latest_timeout field for PA1a, but you need to understand what it does.
    // You may or may not use it in PA1b when you implement fast recovery & fast retransmit in handle_incoming_acks(). 
    // If you choose to retransmit a frame in handle_incoming_acks() in PA1b, all you need to do is:

    // ****************************************
    // long additional_ts = 0; 
    // if (timeval_usecdiff(&curr_timeval, host->latest_timeout) > 0) {
    //     memcpy(&curr_timeval, host->latest_timeout, sizeof(struct timeval)); 
    // }

    //  YOUR FRFT CODE FOES HERE

    // memcpy(host->latest_timeout, &curr_timeval, sizeof(struct timeval)); 
    // timeval_usecplus(host->latest_timeout, additional_ts);
    // ****************************************


    // It essentially fixes the following problem:
    
    // 1) You send out 8 frames from sender0. 
    // Frame 1: curr_time + 0.1 + additional_ts(0.01) 
    // Frame 2: curr_time + 0.1 + additional_ts(0.02) 
    // …

    // 2) Next time you send frames from sender0
    // Curr_time could be less than previous_curr_time + 0.1 + additional_ts. 
    // which means for example frame 9 will potentially timeout faster than frame 6 which shouldn’t happen. 

    // Latest timeout fixes that. 

}

// WE HIGHLY RECOMMEND TO NOT MODIFY THIS FUNCTION
void run_senders() {
    int sender_order[glb_num_hosts]; 
    get_rand_seq(glb_num_hosts, sender_order); 

    for (int i = 0; i < glb_num_hosts; i++) {
        int sender_id = sender_order[i]; 
        struct timeval curr_timeval;

        gettimeofday(&curr_timeval, NULL);

        Host* host = &glb_hosts_array[sender_id]; 

        // Check whether anything has arrived
        int input_cmd_length = ll_get_length(host->input_cmdlist_head);
        int inframe_queue_length = ll_get_length(host->incoming_frames_head);
        struct timeval* next_timeout = host_get_next_expiring_timeval(host); 
        
        // Conditions to "wake up" the host:
        //    1) Acknowledgement or new command
        //    2) Timeout      
        int incoming_frames_cmds = (input_cmd_length != 0) | (inframe_queue_length != 0); 
        long reached_timeout = (next_timeout != NULL) && (timeval_usecdiff(&curr_timeval, next_timeout) <= 0);

        host->awaiting_ack = 0; 
        host->active = 0; 
        host->csv_out = 0; 

        if (incoming_frames_cmds || reached_timeout) {
            host->round_trip_num += 1; 
            host->csv_out = 1; 
            
            // Implement this
            handle_input_cmds(host, curr_timeval); 
            // Implement this
            handle_incoming_acks(host, curr_timeval);
            // Implement this
            handle_timedout_frames(host, curr_timeval);
            // Implement this
            handle_outgoing_frames(host, curr_timeval); 
        }
        
        //Check if we are waiting for acks
        for (int j = 0; j < glb_sysconfig.window_size; j++) {
            
            if (host->send_window[j].frame != NULL) {
                //print random string
                //print random string
                //print host->id
                //print i
               // printf("aaa%daaa",i);
                host->awaiting_ack = 1; 
                break; 
            }
        }
        //print host->id
        //Condition to indicate that the host is active 
        if (host->awaiting_ack || ll_get_length(host->buffered_outframes_head) > 0) {
            //printf host -> awaiting_ack
          //  printf("host->awaiting_ack in sender.c: %d\n", host->awaiting_ack);
            //print ll_get_length(host->buffered_outframes_head)

           // printf("host->buffered_outframes_head in sender.c: %d\n", ll_get_length(host->buffered_outframes_head));
            //printf("host->window_size[0]->frame->data in sender.c: %s\n", host->send_window[0].frame->data);
            host->active = 1; 
        }
    }
}