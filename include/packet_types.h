#ifndef __PACKET_TYPES_H
#define __PACKET_TYPES_H

#include "../include/comm_defs.h"

typedef struct
{
    char dest[IP_LEN];
    char src[IP_LEN];
    char cmd;
    char msg[MSG_LEN];
} TP_Packet;

typedef struct
{
    char final_dest[IP_LEN];
    char curr_src[IP_LEN];
    char start_src[IP_LEN];
    char cmd;
    int hop_cnt;
    TP_Packet tp_pkt;
} RP_Packet;

#endif
