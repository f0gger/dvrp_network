#include <string.h>
#include <stdio.h>
#include "../include/tools.h"

int get_port(char* addr)
{
    int ret_port = 5000;
    
    for (int i = 4; i < 7; i++)
    	ret_port += addr[i] - '0';
    	
    return ret_port;
}

void get_ip(char* ip_ptr, char* srv_addr, int port)
{
    int i;
    for (i = 0; i < 3; i++)
    	ip_ptr[i] = srv_addr[i];
    	
    ip_ptr[i] = '.';	
    
    int tp_crt = port - 5000;

    int num_dig = 0, temp = tp_crt;
    while(temp > 0)
    {
        num_dig++;
        temp /= 10;
    }

    char temp_ip[8];
    strcpy(temp_ip, ip_ptr);
    switch(num_dig)
    {
        case 1:
            strcat(temp_ip, "00");
            temp_ip[6] = '0' + tp_crt;
            break;

        case 2:
            strcat(temp_ip, "0");
            temp_ip[5] = '0' + tp_crt / 10;
            temp_ip[6] = '0' + tp_crt % 10;
            break;

        case 3:
            strcat(temp_ip, "255");
            break;
    }

    temp_ip[7] = '\0';
    strcpy(ip_ptr, temp_ip);
}

//prebaciti u neki header, mozda spojiti sa get_ip
void get_srv_ip(char* ip_addr, char* ip)
{
    int len = strlen(ip_addr);
    
    for (; !((ip_addr[len] >= '0') && (ip_addr[len] <= '9')) ; len--);
    
    int poz = 2;
    for (int i = len; ip_addr[i] != '.'; i--)
    {
    	ip[poz] = ip_addr[i];
    	poz--;
    }	  
}

void make_dest(char* str, TP_Packet* pack)
{
    char pom[5] = ".001";
    for (int i = 0; i < 3; i++)
    	str[i] = pack->dest[i];
    for (int i = 0; i < 5; i++)
    	str[i+3] = pom[i];
    str[8] = '\0';
}

int check_ip_format(char *str)
{
    int i = 0;
    while (str[i] != '\0') {
        if (i == 3) {
            if (str[i] != '.')
                return 1;
        }
        else {
            if ((str[i] < '0') || (str[i] > '9'))
                return 2;
        }
        i++;
    }

    return 0;
}

int find_actual_ip(const char* actual_ip, const char* rp_ip)
{
    int upper_oct1, upper_oct2;
    sscanf(rp_ip, "%d.%*d", &upper_oct1);
    sscanf(actual_ip, "%*d.%*d.%*d.%d", &upper_oct2);

    return (upper_oct1 == upper_oct2);
}