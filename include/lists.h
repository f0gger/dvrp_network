#ifndef __LISTS_H
#define __LISTS_H

#include "../include/comm_defs.h"
//#include <ctime>
#include <time.h>

typedef struct Node
{
    char addr[IP_LEN];
    time_t last_time;
    struct Node *next;
} MAIL_LIST;

void init(MAIL_LIST **head);
MAIL_LIST *make_node(char *addr);
MAIL_LIST* find_node(MAIL_LIST** head, char* addr);
void concat_node(MAIL_LIST **head, MAIL_LIST *new_node);
void add_node(MAIL_LIST **head, char* addr);
void remove_node(MAIL_LIST** head, MAIL_LIST* node);
void output(MAIL_LIST *head);

//ovo je napravljeno slicno kao mail list, mozda moze da se generalise nekako i stavi u odvojen header
typedef struct Router_Node
{
    char ip[IP_LEN];
    char next_hop[IP_LEN];
    int hopcount;
    time_t last_time;
    struct Router_Node* next;
} Router_Table;

void rt_init(Router_Table **head);
Router_Table *rt_make_node(char *ip, char *next_hop, int hopcount);
void rt_concat_node(Router_Table **head, Router_Table *new_node);
Router_Table* rt_check_node(Router_Table** head, char* ip, char *next_hop);
void rt_add_node(Router_Table **head, char* ip, char *next_hop, int hopcount);
Router_Table* rt_find_node(Router_Table** head, char* ip);
void rt_remove_node(Router_Table** head, Router_Table *node);
void rt_output(Router_Table *head);

#endif
