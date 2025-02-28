#include "host.h"
#include <assert.h>
#include "switch.h"
#include <stdbool.h>

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
    /*
    //print next_timeout
    if (next_timeout != NULL) {
        printf("next_timeout in sender.c: %ld\n", next_timeout->tv_usec + next_timeout->tv_sec * 1000000);
    }
    //printf("next_timeout in sender.c: %ld\n", next_timeout->tv_usec + next_timeout->tv_sec * 1000000);
    */
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
   // printf("ll_get_length(host->incoming_frames_head) %d\n", ll_get_length(host->incoming_frames_head));
    while (ll_get_length(host->incoming_frames_head) != 0) {
        
        //printf("HERE??");
        //print inframe->seq_num
        //print ll_get_length(host->incoming_frames_head)
        LLnode* ll_input_cmd_node = ll_pop_node(&host->incoming_frames_head);
        Frame* inframe = (Frame*) ll_input_cmd_node->value;
        if (compute_crc8(convert_frame_to_char(inframe)) != 0){
           // printf("WHY");
            continue;
        }
        else{
           // printf("wow!");
        }
        //print inframe->ack_num
      //  printf("inframe->ack_num: %d\n", inframe->ack_num);
        //print inframe -> src_id
       // printf("asdinframe -> src_id: %d\n", inframe -> src_id);
      //  printf("%i:\n",host -> sendArray[inframe -> dst_id].LAR + 1);
      //  printf("%i:\n",host -> sendArray[inframe -> dst_id].LFS);
        uint8_t k1 = host -> sendArray[inframe -> src_id].LAR;
        uint8_t k2 = host -> sendArray[inframe -> src_id].LFS;
        //print inframe -> seq_num
        
        if(abs(seq_num_diff(inframe->ack_num,k1)+abs(seq_num_diff(inframe->ack_num,k2)) >  abs(seq_num_diff(k1,k2)))){
           //printf("NONONO");
            continue;
        }
        //printf("inframe -> seq_num: %d\n", inframe -> ack_num);
        //print sendArray[inframe -> src_id].LAR
        //printf("sendArray[inframe -> src_id].LAR: %d\n", host -> sendArray[inframe -> src_id].LAR);
        if (inframe -> ack_num == host -> sendArray[inframe -> src_id].LAR){
            num_dup_acks_for_this_rtt[inframe -> src_id] += 1;
        }
        if (num_dup_acks_for_this_rtt[inframe -> src_id] == 3){
            //print host -> cc[inframe -> src_id].cwnd
            //printf("host -> cc[inframe -> src_id].cwnd in sender.c: %f\n", host -> cc[inframe -> src_id].cwnd);
            host -> cc[inframe -> src_id].ssthresh = host -> cc[inframe -> src_id].cwnd / 2;
            host -> cc[inframe -> src_id].cwnd = host -> cc[inframe -> src_id].ssthresh + 3;
        }
        if (num_dup_acks_for_this_rtt[inframe -> src_id] == 4){
            host -> cc[inframe -> src_id].state = cc_FRFT;
        }
        if (num_dup_acks_for_this_rtt[inframe -> src_id] > 3){
            host -> cc[inframe -> src_id].cwnd += 1;
        }
        num_acks_received[inframe -> src_id] += 1;
        //print inframe -> ack_num
        //print host->send_window[i].frame->seq_num
        //printf("inframe->ack_numwhatas: %d\n", host->send_window[0].frame->seq_num);
        for (int i = 0; i < glb_sysconfig.window_size; i++){
            
            if (host->send_window[i].frame != NULL && seq_num_diff(host->send_window[i].frame -> seq_num,inframe -> ack_num) >= 0 && inframe -> is_ack == 1){
                //free(host->send_window[(host -> LAR) % glb_sysconfig.window_size].frame);
                //printf("Correct");
                free(host->send_window[i].frame);
                //print i
                //printf("i: %d\n", i);
                host->send_window[i].frame = NULL;
                host->send_window[i].timeout = NULL;
            }
        }
        //print inframe -> dst_id
       // printf("what the hack");
     //   printf("zxcinframe -> dst_id: %d\n", inframe -> dst_id);
        if (host -> cc[inframe -> src_id].cwnd <= host -> cc[inframe -> src_id].ssthresh){
            host -> cc[inframe -> src_id].cwnd = host -> cc[inframe -> src_id].cwnd + 1;
        }
        else{
            if (inframe -> ack_num > host -> sendArray[inframe -> src_id].LAR){
                host -> cc[inframe -> src_id].cwnd = host -> cc[inframe -> src_id].cwnd + 1 / host -> cc[inframe -> src_id].cwnd;
                host -> cc[inframe -> src_id].state = cc_AIMD;
            }
        }
        host -> sendArray[inframe -> src_id].LAR = inframe -> ack_num;//maybe should be src_id
        //print host -> cc -> cwnd
        //printf("host -> cc -> cwnd in sender.c: %f\n", host -> cc -> cwnd);
        //print host -> sendArray[inframe -> src_id].LAR
       // printf("host -> sendArray[inframe -> src_id].LAR in sender.c: %d\n", inframe -> ack_num);
        //printf("acdwe waht %d",k);
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
       // printf("input_cmd_length in sender.c: %d\n", input_cmd_length);
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
          //  printf(
              //  "<SEND_%d>: sending messages of length greater than %d is not "
             //   "implemented\n",
               // host->id, FRAME_PAYLOAD_SIZE);
            
            
            //mimic the else part, but deal with storing first 64 bytes into buffer[0] and next 64 into buffer[1]
            //and so on

            int num_frames = ceil((double)msg_length / FRAME_PAYLOAD_SIZE);
            //print num_frames
            //print num_frames
           // printf("num_frames in sender.c: %d\n", num_frames);

            for (int i = 0; i < num_frames; i++) {
                Frame* outgoing_frame = calloc(1,sizeof(Frame));
                assert(outgoing_frame);
                outgoing_frame->src_id = outgoing_cmd->src_id;
                outgoing_frame->dst_id = outgoing_cmd->dst_id;
                outgoing_frame->remaining_msg_bytes = max(msg_length - (i + 1)* FRAME_PAYLOAD_SIZE,0);
                
                //print outgoing_frame->remaining_msg_bytes
              // printf("outgoing_frame->remaining_msg_bytes in handle_input_cmds in sender.c: %d\n", outgoing_frame->remaining_msg_bytes);
                uint8_t numbytes = 0;
                if (outgoing_frame->remaining_msg_bytes > 0) {
                    numbytes = FRAME_PAYLOAD_SIZE;
                } else {
                    numbytes = msg_length - i * FRAME_PAYLOAD_SIZE;
                }
                outgoing_frame->starting_index = i * FRAME_PAYLOAD_SIZE;
                outgoing_frame->len = numbytes;
               
              //  printf("outgoing_frame->remaining_msg_bytes in handle_input_cmds: %d\n", outgoing_frame->remaining_msg_bytes);
                memcpy(outgoing_frame->data, outgoing_cmd->message + i * FRAME_PAYLOAD_SIZE, numbytes);
               // printf("whainframe -> data:%s\n",outgoing_frame -> data);
                //printf("wha1inframe -> len:%d\n",outgoing_frame -> len);
               // printf("wha1inframe -> starting_index:%d\n",outgoing_frame -> starting_index);
               host -> sendArray[outgoing_frame -> dst_id].LFS = host -> sendArray[outgoing_frame -> dst_id].LFS + 1;
            //print out_frame -> dst_id
           // printf("out_frame -> dst_id in sender.c: %d\n", outgoing_frame -> dst_id);
          //  printf("after host -> sendArray[outgoing_frame -> src_id].LFS:%i\n",host -> sendArray[outgoing_frame -> dst_id].LFS);
            outgoing_frame->seq_num = host->sendArray[outgoing_frame->dst_id].LFS;
            outgoing_frame -> is_ack = 0;
                ll_append_node(&host->buffered_outframes_head, outgoing_frame);
            //printf("outgoing_frame->data in handle_input_cmds: %s\n", outgoing_frame->data);
               // printf("pmace holder");
            } 
           //print ll_get_length(host->buffered_outframes_head)
           
        } else {
            Frame* outgoing_frame = calloc(1,sizeof(Frame));
            assert(outgoing_frame);
            strcpy(outgoing_frame->data, outgoing_cmd->message);
            //print outgoing_frame->data
            outgoing_frame->remaining_msg_bytes = 0;
            outgoing_frame->src_id = outgoing_cmd->src_id;
            outgoing_frame->dst_id = outgoing_cmd->dst_id;
            outgoing_frame->starting_index = 0;
            outgoing_frame->len = msg_length;
            //print outgoing_frame->len
           // printf("love1inframe -> data:%s\n",outgoing_frame -> data);
           // printf("love1inframe -> len:%d\n",outgoing_frame -> len);
           // printf("love1inframe -> starting_index:%d\n",outgoing_frame -> starting_index);
            // At this point, we don't need the outgoing_cmd
            free(outgoing_cmd->message);
            free(outgoing_cmd);
            host -> sendArray[outgoing_frame -> dst_id].LFS = host -> sendArray[outgoing_frame -> dst_id].LFS + 1;
            //print out_frame -> dst_id
           // printf("out_frame -> dst_id in sender.c: %d\n", outgoing_frame -> dst_id);
          //  printf("after host -> sendArray[outgoing_frame -> src_id].LFS:%i\n",host -> sendArray[outgoing_frame -> dst_id].LFS);
            outgoing_frame->seq_num = host->sendArray[outgoing_frame->dst_id].LFS;
            outgoing_frame -> is_ack = 0;
            
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
   // printf("curr_timeval in sender.c: %ld\n", curr_timeval.tv_usec + curr_timeval.tv_sec * 1000000);
    for (int i = 0; i < glb_sysconfig.window_size; i++) {
        if (host->send_window[i].timeout != NULL && timeval_usecdiff(&curr_timeval, host->send_window[i].timeout) <= 0) {
         //   printf("Here");
            //print i
          //  printf("%d", i);
        //    printf("there\n");
            host->send_window[i].timeout = NULL;

        }
    }
    
}

void handle_outgoing_frames(Host* host, struct timeval curr_timeval) {

    long additional_ts = 0; 

    if (timeval_usecdiff(&curr_timeval, host->latest_timeout) > 0) {
        memcpy(&curr_timeval, host->latest_timeout, sizeof(struct timeval)); 
    }
    for (int i = 0; i < glb_sysconfig.window_size; i++){
        if (host->send_window[i].frame != NULL && host -> send_window[i].frame -> seq_num == host -> sendArray[host -> send_window[i].frame -> dst_id].LAR + 1 ){
            if (host -> cc[host -> send_window[i].frame -> dst_id].state == cc_FRFT){
                host -> cc[host -> send_window[i].frame -> dst_id].cwnd = host -> cc[host -> send_window[i].frame -> dst_id].ssthresh;
                struct timeval* next_timeout = malloc(sizeof(struct timeval));
                memcpy(next_timeout, &curr_timeval, sizeof(struct timeval)); 
                timeval_usecplus(next_timeout, TIMEOUT_INTERVAL_USEC + additional_ts);
                Frame * cop = calloc (1,sizeof(Frame));
                memset(cop, 0, sizeof(Frame));
                host->send_window[i].timeout = next_timeout;//(this is what I add)
                memcpy(cop, host->send_window[i].frame, sizeof(Frame));
            //printf("resendhost->send_window[i].frame->data in sender.c: %s\n", host->send_window[i].frame->data);
                ll_append_node(&host->outgoing_frames_head, cop);
                additional_ts += 10000; //ADD ADDITIONAL 10ms
            }
            
        }
    }
    //TODO: Send out the frames that have timed out(i.e. timeout = NULL)
    
    for (int i = 0; i < glb_sysconfig.window_size; i++) {
        if (host->send_window[i].timeout == NULL && host->send_window[i].frame != NULL){
          //  printf("BCGDDAA\n");
          ///  printf("%d\n",i);
          //  printf("BCGDDAA\n");
         //  printf("awfeqfwqdwqhost->send_window[i].frame->data in sender.c: %d\n", host->send_window[i].frame -> dst_id);
         //   printf("awfeqfwqdwqhost->send_window[i].frame->data in sender.c: %d\n", host->send_window[i].frame -> src_id);
            //print send_window[i].frame->data
         //   printf("acderoutgoing_frame->data in sender.c: %s\n", host -> send_window[i].frame->data);
          //  printf("host->send_window[i].frame->data in sender.c: %s\n", host->send_window[1].frame -> data);
            struct timeval* next_timeout = malloc(sizeof(struct timeval));
            memcpy(next_timeout, &curr_timeval, sizeof(struct timeval)); 
            timeval_usecplus(next_timeout, TIMEOUT_INTERVAL_USEC + additional_ts);
            Frame * cop = calloc (1,sizeof(Frame));
            memset(cop, 0, sizeof(Frame));
            host->send_window[i].timeout = next_timeout;//(this is what I add)
            memcpy(cop, host->send_window[i].frame, sizeof(Frame));
            //printf("resendhost->send_window[i].frame->data in sender.c: %s\n", host->send_window[i].frame->data);
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
    if (host -> wait == 1){
        for (int i = 0; i < glb_sysconfig.window_size; i++){
            if (host->send_window[i].frame != NULL){
                break;
            }
            if (i == glb_sysconfig.window_size - 1){
                host -> wait = 0;
                
            }
        }
    }   
    host -> c = 0;
   
  //  printf("ll_get_length(host->buffered_outframes_head) in sender.c: %d\n", ll_get_length(host->buffered_outframes_head));
    for (int i = 0; i < glb_sysconfig.window_size && ll_get_length(host->buffered_outframes_head) > 0; i++) {
        //printf("i: %d\n", i);
        if (host->send_window[i].frame == NULL) {
            //print i   
            if (host -> wait == 1 || host -> c == 1){
                break;
            }
            //print ll_get_length(host->buffered_outframes_head)
            //print ll_get_length(host->buffered_outframes_head)
          //  printf("ll_get_length(host->buffered_outframes_head in glb) in sender.c: %d\n", ll_get_length(host->buffered_outframes_head));
            LLnode* ll_outframe_node = ll_pop_node(&host->buffered_outframes_head);
            Frame* outgoing_frame = ll_outframe_node->value; 
            if (ll_get_length(host->buffered_outframes_head)!=0){
                Frame* check = ll_peek_node(host->buffered_outframes_head);
                //print check -> seq_num
                //print outgoing_frame -> seq_num
               // printf("check->seq_num in sender.c: %d\n", check->seq_num);
              //  printf("outgoing_frame->seq_num in sender.c: %d\n", outgoing_frame->seq_num);   
                //print host-> cc -> cwnd
               // printf("avdhost -> cc -> cwnd in sender.c: %f\n", host -> cc -> cwnd);
              //  printf("avdll_get_length(host->buffered_outframes_head) in sender.c: %d\n", ll_get_length(host->buffered_outframes_head));
                //print
               // print check->src_id
                //print check->dst_id
               // print check->data
              //  printf("check->data in sender.c: %s\n", check->data);
              //  printf("check->src_id in sender.c: %d\n", check->src_id);
              //  printf("check->dst_id in sender.c: %d\n", check->dst_id);
                //print host->sendArray[outgoing_frame -> src_id].LAR
                //printf("host->sendArray[outgoing_frame -> src_id].LAR in sender.c: %d\n", host -> sendArray[outgoing_frame -> dst_id].LAR);
                if (seq_num_diff(host -> sendArray[outgoing_frame -> dst_id].LAR,check->seq_num) > 0 && seq_num_diff(check->seq_num,host -> sendArray[outgoing_frame -> dst_id].LAR + host -> cc[outgoing_frame -> dst_id].cwnd) >= 0){
                    host -> c = 0;
                }
                else{
                    host -> c = 1;
                   // printf("no\n");
                }
                if (check  -> dst_id != outgoing_frame -> dst_id && check -> src_id == outgoing_frame -> src_id) {
                    host -> wait = 1;
                    //printf("kiop");
                }
            }
           // print outgoing_frame->data
          //  printf("aqweroutgoing_frame->data in sender.c: %s\n", outgoing_frame->data);
           // print outgoing_frame->data
          //  printf("before host -> sendArray[outgoing_frame -> src_id].LFS:%i\n",host -> sendArray[outgoing_frame -> dst_id].LFS);
            //print host -> outgoing_frames
         //   printf("QAZoutgoing_frame->seq_num: %d\n", outgoing_frame->seq_num);
         //   printf("QAZoutgoing_frame->data in sender.c: %s\n", outgoing_frame->data);
          //  printf("QAZoutgoing_frame->src_id in sender.c: %d\n", outgoing_frame->src_id);
         //   printf("QAZoutgoing_frame->dst_id in sender.c: %d\n", outgoing_frame->dst_id);
         //   printf("testing: %s\n", outgoing_frame -> data);
          //use calloc to create cop
          
          Frame* cop = calloc(1, sizeof(Frame));
            //Frame * cop = (Frame *) malloc (sizeof(Frame));
         //  printf("qwaaaerthost->send_window[i].frame->data in seawernder.c src_id: %d\n", outgoing_frame -> src_id);
         //  printf("qwaaaerthost->send_window[i].frame->data in seawernder.c dst_id: %d\n", outgoing_frame -> dst_id);
            memcpy(cop, outgoing_frame, sizeof(Frame));
         //   printf("qwerthost->send_window[i].frame->data in seawernder.c src_id: %d\n", cop -> src_id);
        //    printf("qwerthost->send_window[i].frame->data in seawernder.c dst_id: %d\n", cop -> dst_id);
            //print outgoing_frame->data
            host -> send_window[i].frame = cop;
            outgoing_frame->crc = 0;
            char* outgoing_charbuf = convert_frame_to_char(outgoing_frame);
            //printf("outgoing_frame in sender.c: %s\n", outgoing_frame -> data);
            outgoing_frame -> crc = compute_crc8(outgoing_charbuf);
            outgoing_charbuf = convert_frame_to_char(outgoing_frame);
            //print outgoing_frame
            
            host -> send_window[i].frame -> crc = outgoing_frame -> crc;
            //printf("outgoing_charbuf in sender.c: %s\n", host -> send_window[i].frame->data);
            
            

            //host->send_window[i].frame -> crc = outgoing_frame -> crc;
            //printf("outgoing_frame in sender.c: %0.55s\n", outgoing_frame -> data);
            ll_append_node(&host->outgoing_frames_head, outgoing_charbuf); 
            //set last element of outgoing_frame -> data to '\0'
            
          //  printf("outgoing_frame -> data in sender.c: %s\n", outgoing_frame -> data);
            //print outgoing_frame -> data
         //   printf("outgoing_frame -> data in sender.c: %s\n", outgoing_frame -> data[strlen(outgoing_frame -> data) - 1]);
        //    printf("outgoing_frame ->crc in sender.c: %d\n", outgoing_frame ->crc); 
            //print outgoing_charbuf
         //   printf("outgoing_charbuf in sender.c: %s\n", outgoing_frame->data);
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
            
            //printf("host->send_window[i].timeout in sender.c: %ld\n", host->send_window[i].timeout->tv_usec + host->send_window[i].timeout->tv_sec * 1000000);
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
            //printf("enter handle input cmds\n");
            // Implement this
            handle_input_cmds(host, curr_timeval); 
            //printf("enter handle_incoming_acks\n");
            // Implement this
            handle_incoming_acks(host, curr_timeval);
           // printf("enter handle timedout frames\n");
            // Implement this
            handle_timedout_frames(host, curr_timeval);
           // printf("enter handle_outgoing_acks\n");
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