#ifndef P2P_H
#define P2P_H

#include	<sys/socket.h> 	/* for socket() and bind() */
#include	<stdio.h>		/* printf() and fprintf() */
#include	<stdlib.h>		/* for atoi() and exit() */
#include	<arpa/inet.h>	/* for sockaddr_in and inet_ntoa() */
#include	<sys/types.h>
#include	<string.h>
#include	<unistd.h>
#include <ctype.h>

#define	ECHOMAX	255		/* Longest string to echo */
#define BACKLOG	128
#define MAX_COMMAND 8
#define MESSAGE_WIDTH 8

// Actions
#define INPUT_ERROR '\0'
#define REGISTER '0'
#define QUERY_PLAYERS '1'
#define START_GAME '2'
#define QUERY_GAMES '3'
#define END_GAME '4'
#define DEREGISTER '5'
#define MAKE_CALL '6'
#define BINGO '7'
#define NOTIFY_BINGO '8'

#define REQUEST '0'
#define REPLY '1'

#define NAME_MAX 32
#define IP_SIZE 16
#define MAX_PORT_LEN 10
#define MAX_STRING_SIZE 64
#define BUFFER_SIZE 512

#define OK 'A'
#define ERROR 'B'
#define FAILURE 'C'

#define MAX_PLAYERS 32
#define MAX_GAMES 32
#define MIN_PLAYERS 2


// #define DEBUG

struct message {
    char action;
    char req;
    char error_code;
    unsigned int data_len;
    char data[BUFFER_SIZE];
};

struct player {
    char name[NAME_MAX];
    char IP[IP_SIZE];
    int port;
};

struct game {
    int id;
    int k;
    struct player players[MAX_PLAYERS];
};


void struct_to_message(struct message m, char *buffer)
{

    #ifdef DEBUG
    printf("STR: %s\n", m.data);
    #endif
    
    if (m.action > NOTIFY_BINGO && m.action < REGISTER)
        return;
    if (m.req != REQUEST && m.req != REPLY)
        return;
    
    int index = 0;

    buffer[index++] = m.action;
    buffer[index++] = '|';
    buffer[index++] = m.req;
    buffer[index++] = '|';
    buffer[index++] = m.error_code;
    buffer[index++] = '|';

    char *ptr = &buffer[index];
    strncpy(ptr, m.data, m.data_len);
    index += m.data_len;
    buffer[index] = '\0';

    #ifdef DEBUG
    printf("STRING: %s\n", buffer);
    #endif
}
int get_int(char *ptr)
{
    char buffer[32];
    memset(buffer, '\0', 32);
    int i = 0;
    while (isdigit(*ptr))
    {
        buffer[i++] = *ptr++;
    }
    return atoi(buffer);
}

struct message message_to_struct(char *message_str)
{
    struct message m;
    m.action = message_str[0];
    m.req = message_str[2];
    m.error_code = message_str[4];
    char *ptr = &message_str[6];
    strcpy(m.data, ptr);
    m.data_len = strlen(m.data);
    return m;

}

void DieWithError(const char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

#endif