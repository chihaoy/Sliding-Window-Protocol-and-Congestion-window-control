#include "host.h"
#include "sender.h"
#include "receiver.h"
#include "switch.h"
#include <assert.h>

void init_host(Host* host, int id) {
    host->id = id;
    host->active = 0; 
    host->awaiting_ack = 0; 
    host->round_trip_num = 0; 
    host->csv_out = 0; 
    host->wait = 0;
    //host->LAR = -1;
    //host->LFS = -1;
    //host->LFR = 0;
    //host->NFE = 0;
    //host->crc = 0;
    host->input_cmdlist_head = NULL;
    host->incoming_frames_head = NULL; 
    host->buffered_outframes_head = NULL; 
    host->outgoing_frames_head = NULL; 
    host->send_window = calloc(glb_sysconfig.window_size, sizeof(struct send_window_slot)); 
    //host->receive_window = calloc(glb_sysconfig.window_size, sizeof(struct send_window_slot)); 
    for (int i = 0; i < glb_num_hosts; i++) {
        host->recvArray[i].receive_window = calloc(glb_sysconfig.window_size, sizeof(struct send_window_slot));
    }
    for (int i = 0; i < glb_sysconfig.window_size; i++) {
        host->send_window[i].frame = NULL;
        host->send_window[i].timeout = NULL;
        
    }
    for (int i = 0; i < glb_num_hosts; i++) {
        for (int j = 0; j < glb_sysconfig.window_size; j++){
            host -> recvArray[i].receive_window[j].frame = NULL;
            host -> recvArray[i].receive_window[j].timeout = NULL;
        }
    }
    memset(host->emptyCharArray, 0, sizeof(host->emptyCharArray));
    
    host->latest_timeout = malloc(sizeof(struct timeval));
    gettimeofday(host->latest_timeout, NULL);
    
    // TODO: You should fill in this function as necessary to initialize variables
    //host->recvArray[0].sendQ[0]->frame = NULL;
    for (int i = 0; i < glb_num_hosts; i++) {
        host->recvArray[i].receive_window = calloc(glb_sysconfig.window_size,sizeof(struct send_window_slot));
        
    }
    for (int i = 0; i < glb_num_hosts; i++) {
        for (int j = 0; j < glb_sysconfig.window_size; j++) {
            host->recvArray[i].receive_window[j].frame = NULL;
            host->recvArray[i].receive_window[j].timeout = NULL;
            host->recvArray[i].NFE = 0;
            host->recvArray[i].LFR = 0;
           
            host->sendArray[i].LAR = -1;
            host->sendArray[i].LFS = -1;
        }
    }
    // *********** PA1b ONLY ***********
    host->cc = calloc(glb_num_hosts, sizeof(CongestionControl));
    for (int i = 0; i < glb_num_hosts; i++) {
        host->cc[i].cwnd = 1.0; 
        host->cc[i].ssthresh = (double)glb_sysconfig.window_size; 
        host->cc[i].dup_acks = 0; 
        host->cc[i].state = cc_SS; 
    }
}

void run_hosts() {
    run_senders(); 
    send_data_frames(); 
    run_receivers(); 
   
    send_ack_frames(); 
}