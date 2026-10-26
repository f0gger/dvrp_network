#ifndef __TOOLS_H
#define __TOOLS_H

#include "../include/packet_types.h"

int get_port(char* addr);
void get_ip(char* ip_ptr, char* srv_addr, int port);
void get_srv_ip(char* ip_addr, char* ip);
void make_dest(char* str, TP_Packet* pack);
int check_ip_format(char* str);
int find_actual_ip(const char* actual_ip, const char* rp_ip);

#endif
