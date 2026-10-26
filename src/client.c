#include <asm-generic/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <pthread.h>
#include <semaphore.h>

#include "../include/err_codes.h"
#include "../include/packet_types.h"
#include "../include/tools.h"
#include "../include/timerfd.h"

#define IP_ADDRESS "127.0.0.1"
#define PORT 5001
#define INBOX_SIZE 10

#define NETMASK    "001."
#define SRV_ADDR   "001.001"
#define START_PORT 5002

int client_socket_fd, recv_sock_fd;
struct sockaddr_in servaddr, myaddr, recv_addr;
char tp_ip[IP_LEN];
char srv_addr[IP_LEN] = "000.001";

char* inbox[INBOX_SIZE];

int post_status, should_ping;
static pthread_mutex_t post_mux;
static pthread_mutex_t inbox_mux;
static pthread_mutex_t ping_mux;

struct sockaddr_in get_srv_addr(struct sockaddr_in srv)
{
    memset(&srv, 0, sizeof(srv));
    
    srv.sin_family = AF_INET;
    srv.sin_port = htons(PORT);
    srv.sin_addr.s_addr = inet_addr(IP_ADDRESS);
    
    return srv;
}

int get_my_addr(int sock_fd, struct sockaddr_in my, int* port)
{
    memset(&my, 0, sizeof(my));
         
    my.sin_family = AF_INET;
    my.sin_addr.s_addr = inet_addr(IP_ADDRESS);
    my.sin_port = htons(*port);
    while (bind(sock_fd, (const struct sockaddr *)&my, sizeof(my)) < 0)
    {
    	if ((*port) % 1000 > 255)
    	    return -1;
        (*port)++;
        my.sin_port = htons(*port);
    }
    return 0;
}

void make_msg(TP_Packet* tp_pkt, char* dest, char* src, char cmd, char* msg_buff)
{
    strcpy(tp_pkt->dest, dest);
    strcpy(tp_pkt->src, src);
    tp_pkt->cmd = cmd;
    strcpy(tp_pkt->msg, msg_buff);
}

int send_msg(char* dest, char* src, char cmd, char* msg_buff)
{
    TP_Packet* snd = malloc(sizeof(TP_Packet));
    make_msg(snd, dest, src, cmd, msg_buff);
    int router_msg = 0;
    
    ssize_t sent_size, recv_size;
    socklen_t len;
    len = sizeof(servaddr);
    
    if ((sent_size = sendto(client_socket_fd, (const TP_Packet*)snd, sizeof(TP_Packet), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr))) == sizeof(TP_Packet))
    {    
    	//free(snd);
        char recv_buff;
        if ((recv_size = recvfrom(client_socket_fd, (char *)&recv_buff, strlen(&recv_buff), 0, (struct sockaddr *)&servaddr, &len)) >= 0)
            router_msg = recv_buff;
        else
            router_msg = RECVFROM_FAIL;
    }
    else
        router_msg = SENDTO_FAIL;
      
    free(snd);
    return router_msg;
}

void* pingServ(void* param)
{
    TP_Packet* tp_alive = (TP_Packet *)malloc(sizeof(TP_Packet));
    strcpy(tp_alive->dest, srv_addr);
    strcpy(tp_alive->src, tp_ip);
    tp_alive->cmd = 'h';
    strcpy(tp_alive->msg, "\0");

    periodic_info p_info;
    ssize_t sent_size;

    int ping = 0;

    while (!ping)
    {
        pthread_mutex_lock(&ping_mux);
        ping = should_ping;
        pthread_mutex_unlock(&ping_mux);
    }


    make_periodic(10000000, &p_info, 1);
    while (ping)
    {    	       
        pthread_mutex_lock(&ping_mux);
        ping = should_ping;
        pthread_mutex_unlock(&ping_mux);   

    	if ((sent_size = sendto(client_socket_fd, (const TP_Packet *)tp_alive, sizeof(TP_Packet), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr))) == sizeof(TP_Packet))
    	{
    	    //printf("Sent message size %ld bytes\n", sent_size);
    	    //printf("Sent message to %s:%hu\n", inet_ntoa(servaddr.sin_addr), htons(servaddr.sin_port));
    	}
    	else
    	{
    	    printf("Router not responding, removing from list\n");
    	}
    	    //pthread_mutex_unlock(&port_mux); 
    	wait_period(&p_info);
    }

    free(tp_alive);
    return  0;
}

void* recv_msg(void* param)
{
    ssize_t recv_size;
    socklen_t len = sizeof(servaddr);
    int cond = 0;
    int index = 0;
    
    char msg[MSG_LEN];
    
    while (1)
    {   	   
    	while (cond != 1)
    	{
            pthread_mutex_lock(&post_mux);
            cond = post_status;
            pthread_mutex_unlock(&post_mux);
            
            if (cond == 3)
            	return 0;
        }
        cond = 0;
                                       
        if ((recv_size = recvfrom(recv_sock_fd, (char *)msg, MSG_LEN, MSG_DONTWAIT, (struct sockaddr *)&servaddr, &len)) >= 0)
        {   
            msg[recv_size] = '\0';
               	
            pthread_mutex_lock(&inbox_mux);
            strcpy(inbox[index], msg);
            pthread_mutex_unlock(&inbox_mux);
           
            index = (index + 1) % INBOX_SIZE;
        }
    }

    //free(recv_pkt);
    return 0;
}

void* worker(void* param)
{
    char cmd;
    char c_buff[3];
    char dest[IP_LEN];
    char msg[MSG_LEN];
    int check, should_send;
    
    int isOpen = 0;
    int exit = 0;
    while (!exit)
    {        	
    	printf("Choose a command:\n1.send message| 2.open post | 3.close post | 4.check inbox | 5.exit\n");
    	
    	should_send = 0;

        if (!fgets(c_buff, sizeof(c_buff), stdin))
            continue;

        if (!strchr(c_buff, '\n')) {
            char ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
        }

    	switch (c_buff[0])
    	{
    	    case '1':
    	    	cmd = 's';  
    	    	  	    	
    	    	printf("Choose the destination address (format rrr.hhh): ");
    	    	fgets(dest, IP_LEN, stdin);
                if (!strchr(dest, '\n')) {
                    char ch;
                    while ((ch = getchar()) != '\n' && ch != EOF);
                }
                dest[7] = '\0';
                if (check_ip_format(dest) != 0)
                {
                    printf("IP format is incorrect\n");
                    break;
                } 	    	
    	    	printf("Write a message (max size 64): ");
    	    	fgets(msg, MSG_LEN, stdin);
    	    	
    	    	//padding
    	    	for (int i = strlen(msg) - 1; i < MSG_LEN; i++) {
    	    	    msg[i] = ' ';   	 
                }   	
                msg[MSG_LEN] = '\n';
   	    	
    	    	should_send = 1; 	    	
    	    	break;
    	    	
    	    case '2':    	    	   	    
    	        pthread_mutex_lock(&post_mux);
    		    post_status = 1;
    		    pthread_mutex_unlock(&post_mux);
    		
    		    if (!isOpen)
    		    {
    		        printf("opening post\n");
    		        isOpen = 1;
    	    	    cmd = 'o';
    	    	    strcpy(dest, srv_addr);
    	    	    strcpy(msg, "0");
    	    	    should_send = 1;

                    pthread_mutex_lock(&ping_mux);
                    should_ping = 1;
                    pthread_mutex_unlock(&ping_mux);
    	    	} 
    	    	break;
    	    	
    	    case '3':
    	        pthread_mutex_lock(&post_mux);
    		    post_status = 0;
    		    pthread_mutex_unlock(&post_mux);
    	    
    	    	if (isOpen)
    	    	{
    	    	    isOpen = 0;
    	    	    cmd = 'c';
    	    	    strcpy(dest, srv_addr);
    	    	    strcpy(msg, "0");
    	    	    should_send = 1;
    	    	    printf("closing post\n");

                    pthread_mutex_lock(&ping_mux);
                    should_ping = 0;
                    pthread_mutex_unlock(&ping_mux);
    	    	}
    	    	break;
    	    	
    	    case '4':  	    	
    	    	pthread_mutex_lock(&inbox_mux);
    	    	for (int i = 0; i < INBOX_SIZE; i++)
    	    	    printf("%s\n", inbox[i]);
    	    	pthread_mutex_unlock(&inbox_mux);
    	    	break;
    	    	
    	    case '5':
    	    	exit = 1;
    	    	
    	    	pthread_mutex_lock(&post_mux);
    		    post_status = 3;
    		    pthread_mutex_unlock(&post_mux); 
    	    	
    	    	if (isOpen)
    	    	{
    	    	    isOpen = 0;
    	    	    cmd = 'c';
    	    	    strcpy(dest, srv_addr);
    	    	    strcpy(msg, "\0");
    	    	    should_send = 1;

                    pthread_mutex_lock(&ping_mux);
                    should_ping = 0;
                    pthread_mutex_unlock(&ping_mux);
    	    	}
    	    	break;    	    
    	    		
    	    default:
    	    	printf("Invalid command\n");
    	    	break;
    	}
    	
    	if (should_send)
    	{
    	    check = send_msg(dest, tp_ip, cmd, msg);
    	    printf("done sending\n");
    	    if (check == SENDTO_FAIL)
    	    	printf("Message couldn't be sent %d\n", check);  
    	    else if (check == RECVFROM_FAIL)
    	    	printf("Server was unable to respond %d\n", check);
    	}
    }
    
    return 0;
}

int main()
{
    int return_value = EXIT_SUCCESS;
    int port = START_PORT;
    int recv_port = START_PORT + 1000;
    printf("%d\n", recv_port);
    
    post_status = 0;
    should_ping = 0;

    for (int i = 0; i < INBOX_SIZE; i++)
    	inbox[i] = malloc(MSG_LEN);

    pthread_t hWorker;
    pthread_t hRecv_msg;
    pthread_t hPing;

    char srv_ip[16];
    printf("Enter your ip addr: ");
    fgets(srv_ip, 16, stdin);
    get_srv_ip(srv_ip, srv_addr);
    printf("%s\n", srv_addr);
            
    if ((client_socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        return_value = EXIT_FAILURE;
    }
    else
    {
        servaddr = get_srv_addr(servaddr);
            
        if (get_my_addr(client_socket_fd, myaddr, &port))
        {
            printf("Err: All addresses taken\n");
            close(client_socket_fd);
            return -1;
        }

        struct timeval tout = {5, 0};
        setsockopt(client_socket_fd, SOL_SOCKET, SO_RCVTIMEO, &tout, sizeof(tout));
	    printf("Send_sock_fd done\n");

	    if ((recv_sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	    {
            perror("socket creation failed");
            return_value = EXIT_FAILURE;
	    }
	    else
	    {
            if (get_my_addr(recv_sock_fd, recv_addr, &recv_port))
            {
            	printf("Err: All addresses taken\n");
            	close(recv_sock_fd);
            	return -1;
            }


            get_ip(tp_ip, srv_addr, port);
        
            printf("%s\n", tp_ip);
       
            pthread_mutex_init(&post_mux, NULL);
            pthread_mutex_init(&inbox_mux, NULL);
            pthread_mutex_init(&ping_mux, NULL);
        
            pthread_create(&hRecv_msg, NULL, recv_msg, 0);
            pthread_create(&hWorker, NULL, worker, 0);
            pthread_create(&hPing, NULL, pingServ, 0);
        
            pthread_join(hRecv_msg, NULL);
            pthread_join(hWorker, NULL);
            pthread_join(hPing, NULL);
        
            pthread_mutex_destroy(&ping_mux);
            pthread_mutex_destroy(&post_mux);
            pthread_mutex_destroy(&inbox_mux);

	        close(recv_sock_fd);
            close(client_socket_fd);
	    }
    }

    return return_value;
}
