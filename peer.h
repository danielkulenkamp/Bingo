#ifndef PEER_H
#define PEER_H
#include "p2p.h"

#include <pthread/pthread.h>
#include <errno.h>
#include <sys/poll.h>
#include "bingo.h"

// #define TEST

char  peer_buffer[BUFFER_SIZE];
char managerIP[] = "127.0.0.1";
char manager_port[] = "10000";

char name[NAME_MAX];
char IP[IP_SIZE];
char port[MAX_PORT_LEN];
int basePort;

struct game current_game;
struct bag *bingo_bag;

char player_buffers[MAX_PLAYERS][BUFFER_SIZE];
int player_socks[MAX_PLAYERS];
struct pollfd ufds[MAX_PLAYERS];


void str_cli(FILE *fp, int sockfd);
struct message process_command(char *message);
char *get_token(char *);
int process_reply(char *reply_buffer);
void print_help_menu();
void *listen_on_default(void *arg);
void *listen_on_new(void *arg);

void save_game_info(char *info);

int count_players(char *player_list);

int main(int argc, char **argv);

#endif