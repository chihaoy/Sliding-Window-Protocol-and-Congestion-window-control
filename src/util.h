#ifndef __UTIL_H__
#define __UTIL_H__

#include "common.h"

#include <math.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
// Linked list functions
int ll_get_length(LLnode*);
void* ll_peek_node(LLnode*); 
void ll_append_node(LLnode**, void*);
LLnode* ll_pop_node(LLnode**);
void ll_destroy_node(LLnode*);

// Print functions
void print_cmd(Cmd*);
void print_frame(Frame*); 

// Time functions
void timeval_usecplus(struct timeval*, long); 
long timeval_usecdiff(struct timeval*, struct timeval*);

char* convert_frame_to_char(Frame*);
Frame* convert_char_to_frame(char*);

void frame_sanity_check(Frame*); 
char* cc_state_to_char(enum CCState);
int seq_num_diff(uint8_t, uint8_t); 

// CRC-8 Computation
uint8_t compute_crc8(char* frameChar);

int min(int, int); 
double min_double(double, double); 
int max(int, int); 
double max_double(double, double); 
bool swpInWindow(uint8_t seqno, uint8_t min,
                        uint8_t max);
#endif
