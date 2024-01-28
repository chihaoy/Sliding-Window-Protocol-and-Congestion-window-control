[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/dSnIhH97)
Name:Chihao Yu

PID:A16248350

remaining message bytes:the remaining message bytes that need to be prepared
dst_id,src_id,seq_num is necessary
len:length that needs to be appended
starting_index:The place where I should append message in empty_message_buffer
ack_num:ack number
is_ack:check whether this a ack or not
data[53]:store data from the frame
crc:used to check corruption
sendArray1:array length of 256 and for example, send_Array[i] store the LAR and LFS corresponds to the receiver that has number i
recvArray1:store the NFE and LFR and LAR corresponds to ith host in the sender side and for each of these receive windows it store that has window size
wait:check that we need to wait to get to process talking to different receivers for the same sender until the window clears out.

The whole flow of the project goes like this, first handle input commands handle the input by splitting the messahe and stores to the buffer_head, then the handle_outgoing_frames get the message from the beffer_head, put it in the window and put it in the outgoing_frame_head, at the same time, we compute the crc value, then in the handle incoming frames in the receiver side, I first check if the frame is corrupted or if it is out of window, if it is, I resend the ack back, otherwise, I get the cumulative ack by increase the index for the window size until it is null. then I send the ack frame back to the sender part. Then in the handle incoming ack function, I use the cumulative ack to free the window by free every window which their seq number is less than the ack_num, and then I also update the LAR value. 
