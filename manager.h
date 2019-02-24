#include "p2p.h"

#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include "random_bag.h"


// #define TEST

char manager_buffer[BUFFER_SIZE];
pthread_mutex_t manager_buffer_mutex;



struct player players[MAX_PLAYERS];
int num_players;

struct game games[MAX_GAMES];
int num_games;
int socket_ID;

void print_players();
int register_player(char *name, char *ip_address, char *port);
void deregister_player(char *name);
void process_command(struct message m);
char *get_token(char *);
void send_data(char *, int);
void copy_player_info(char *dest);
int start_game(int k, char *game_info, int size, char *requester);
int copy_game_info(char *dest, int index);
void copy_all_game_info(char *dest);
int end_game(int k);
void *RunManager(void *);
void run_server(int argc, char **argv);
int main(int argc, char **argv);
