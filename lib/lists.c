#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "../include/lists.h"

void init(MAIL_LIST **head)
{
    *head = NULL;
}

MAIL_LIST *make_node(char *addr)
{
    MAIL_LIST *new = (MAIL_LIST *)malloc(sizeof(MAIL_LIST));

    if(new == NULL)
    {
        printf("unable to alocate memory!\n");
        exit(EXIT_FAILURE);
    }

    strcpy(new->addr, addr);
    new->last_time = time(NULL);
    new->next = NULL;

    return new;
}

MAIL_LIST* find_node(MAIL_LIST** head, char* addr)
{    
    MAIL_LIST* tmp = *head;
    printf("finding\n");
    
    while (tmp != NULL)
    {
    	if (strcmp(tmp->addr, addr) == 0)
    	    break;
    	tmp = tmp->next;
    }
    
    return tmp;
}

void concat_node(MAIL_LIST **head, MAIL_LIST *new_node)
{
    MAIL_LIST *temp = *head;

    while (temp->next != NULL) {
    	temp = temp->next;
    }
    temp->next = new_node;
}

void add_node(MAIL_LIST **head, char* addr)
{
    MAIL_LIST* new_node = make_node(addr);
    if(*head == NULL)
    {
        new_node->next = *head;
        *head = new_node;
    }
     
    else
    {
    	MAIL_LIST* temp = *head;
    	
    	while (temp->next != NULL)
    	{
    	    /*if (strcmp(temp->addr, "0") == 0)
    	    {
    	    	strcpy(temp->addr, addr);
    	    	return;
    	    }*/
    	    temp = temp->next;
    	}
    	temp->next = new_node;
    }
}

void remove_node(MAIL_LIST** head, MAIL_LIST* node)
{
    if (*head == NULL || node == NULL)
    	return;
    	
    if (*head == node)
    {
        *head = (*head)->next;
        free(node);
        return;
    }

    MAIL_LIST* tmp = *head;
    
    while (tmp->next != node)
    {
        //couldnt find node for some reason
        if (tmp->next == NULL)
            return;
    	tmp = tmp->next;
    }
    
    tmp->next = node->next;
    free(node);
}

void output(MAIL_LIST *head)
{
    MAIL_LIST *temp = head;

    while (temp != NULL) 
    {
        printf("%s ", temp->addr);
        printf("%ld\n", temp->last_time);
        temp = temp->next;
    }
}

void rt_init(Router_Table **head)
{
    *head = NULL;
}

Router_Table *rt_make_node(char *ip, char *next_hop, int hopcount)
{
    Router_Table *new = (Router_Table *)malloc(sizeof(Router_Table));

    if(new == NULL)
    {
        printf("unable to alocate memory!\n");
        exit(EXIT_FAILURE);
    }

    strcpy(new->ip, ip);
    strcpy(new->next_hop, next_hop);
    new->hopcount = hopcount;
    new->last_time = time(NULL);
    new->next = NULL;

    return new;
}

void rt_concat_node(Router_Table **head, Router_Table *new_node)
{
    Router_Table *temp = *head;

    while (temp->next != NULL) {
    	temp = temp->next;
    }
    temp->next = new_node;
}

Router_Table* rt_check_node(Router_Table** head, char* ip, char *next_hop)
{
    //Router_Table* found = NULL;

    Router_Table* tmp = *head;
    
    while (tmp != NULL)
    {
    	printf("Comparing '%s' with '%s'\n", tmp->ip, ip);
    	if ((strcmp(tmp->ip, ip) == 0) && (strcmp(tmp->next_hop, next_hop) == 0))
    	    break;
    	tmp = tmp->next;
    }
    
    return tmp;
}

void rt_add_node(Router_Table **head, char* ip, char *next_hop, int hopcount)
{
    if (hopcount > 20)
    {
    	printf("Hopcount too big\n");
    	return;
    }
    
    printf("Trying to add: %s %d\n", ip, hopcount);
    Router_Table* new_node = rt_make_node(ip, next_hop, hopcount);

    // Ako je lista prazna ili novi ima manji hopcount
    if (*head == NULL) //|| (*head)->hopcount > hopcount)
    {
        new_node->next = *head;
        *head = new_node;
        return;
    }

    Router_Table* current = *head;

    while (current->next != NULL)
        current = current->next;

    current->next = new_node;
    /*Router_Table* prev = NULL;

    // Prolazi kroz listu dok je sledeći čvor manji ili jednak
    while (current != NULL && current->hopcount <= hopcount)
    {
    	printf("Comparing '%s' with '%s'\n", current->ip, ip);
        // Ako već postoji čvor sa istim IP, preskoči/dodaj samo ako je bolji hopcount
        if (strcmp(current->ip, ip) == 0)
        {
            if (hopcount < current->hopcount)
            {
                current->hopcount = hopcount; // ažuriraj postojećeg
            }
            free(new_node); // već postoji — ne dodaj duplikat
            return;
        }

        prev = current;
        current = current->next;
    }

    // Ubaci novi čvor između prev i current
    new_node->next = current;
    prev->next = new_node;*/
}

Router_Table* rt_find_node(Router_Table** head, char* ip)
{    
    Router_Table* tmp = *head;
    
    while (tmp != NULL)
    {
    	printf("Comparing '%s' with '%s'\n", tmp->ip, ip);
    	if (strcmp(tmp->ip, ip) == 0)
    	    break;
    	tmp = tmp->next;
    }
    
    return tmp;
}

void rt_remove_node(Router_Table** head, Router_Table *node)
{
    if (*head == NULL || node == NULL)
    	return;
    	
    if (*head == node)
    {
        *head = (*head)->next;
        free(node);
        return;
    }

    Router_Table* tmp = *head;
    
    while (tmp->next != node)
    {
        //couldnt find node for some reason
        if (tmp->next == NULL)
            return;
    	tmp = tmp->next;
    }
    
    tmp->next = node->next;
    free(node);
}

void rt_output(Router_Table *head)
{
    Router_Table *temp = head;

    while (temp != NULL) 
    {
        printf("%s ", temp->ip);
        printf("%s", temp->next_hop);
        printf("%d ", temp->hopcount);
        printf("%ld\n", temp->last_time);
        temp = temp->next;
    }
}
