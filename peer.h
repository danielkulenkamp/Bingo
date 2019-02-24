#ifndef PEER_H
#define PEER_H
#include "p2p.h"

#include <pthread.h>
#include <errno.h>
#include <sys/poll.h>
#include "bingo.h"

// #define TEST
#define FINAL

char  peer_buffer[BUFFER_SIZE];
char managerIP[IP_SIZE];
char manager_port[MAX_PORT_LEN];;

char myName[NAME_MAX];
bool myNameSet;

char name[NAME_MAX];
char IP[IP_SIZE];
char port[MAX_PORT_LEN];
int basePort = 20000;

struct game current_game;
struct bag *bingo_bag;

char player_buffers[MAX_PLAYERS][BUFFER_SIZE];
int player_socks[MAX_PLAYERS];
struct pollfd ufds[MAX_PLAYERS];


void str_cli(FILE *fp, int sockfd);
char *get_token(char *str);
struct message process_command(char *message);
int process_reply(char *reply_buffer);
void print_help_menu();
void *listen_on_new(void *arg);
void *listen_on_default(void *arg);
int count_players(char *player_list);
int Register(char *name, char *ip, char *port);
int DeRegister(char *name);
int QueryPlayers();
int StartGameWithManager(int num_players);
int EndGameWithManager(int id);
int save_game_info(char *info);

void SetupBingo();
void TearDownBingo();
void InitiateTCPConnections();
void CloseTCPConnections();
void MakeCall();
void SendBingo(char *winner);
bool CheckWinner();
void *Caller(void *arg);
bool verifyIP(char *ip);
bool verifyPort(char *port);


int main(int argc, char **argv);

#endif