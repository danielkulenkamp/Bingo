

#include "manager.h"

void print_players()
{
    extern int num_players;
    extern struct player players[];
    if (num_players == 0)
        printf("No players registered\n");
    for (int i = 0; i < num_players; i++)
        printf("Player: %s, %s, %d\n", players[i].name, players[i].IP, players[i].port);
}

void register_player(char *name, char *ip_address, char *port)
{
    extern int num_players;
    extern struct player players[];

    strncpy(players[num_players].name, name, strlen(name));
    strncpy(players[num_players].IP, ip_address, IP_SIZE);
    players[num_players].port = atoi(port);

    num_players += 1;

    printf("Registering player: %s, %s, %s\n", name, ip_address, port);

    #ifdef DEBUG
    print_players();
    #endif
}

void deregister_player(char *name)
{
    extern int num_players;
    extern struct player players[];
    
    int found_index = -1;

    for (int i = 0; i < num_players; i++)
    {
        printf("HI\n");
        #ifdef DEBUG
        printf("to delete: %s, length: %d\n", name, strlen(name));
        printf("current: %s, len: %d\n", players[i].name, strlen(players[i].name));
        #endif
        if (strcmp(players[i].name, name) == 0)
        {
            // transfer last player in list to this position
            // need to fix this, num_players points to next free position in the array
            // so it's not doing anything transferring the 
            // data over
            memset(players[i].name, '\0', NAME_MAX);
            memset(players[i].IP, '\0', IP_SIZE);
            strncpy(players[i].name, players[num_players].name, strlen(players[num_players].name));
            strncpy(players[i].IP, players[num_players].IP, IP_SIZE);
            players[i].port = players[num_players].port;

            memset(players[num_players].name, '\0', NAME_MAX);
            memset(players[num_players].IP, '\0', IP_SIZE);
            players[num_players].port = -1;            
            num_players --;
            printf("Player Deregistered\n");
            #ifdef DEBUG
            print_players();
            #endif
            return;
        } 
    }

    printf("Player not found\n");
}

void process_command(struct message m)
{
    #ifdef DEBUG
    printf("manager.c - process_command: Splitting first token\n");
    #endif
    char name[NAME_MAX];
    char ip_address[IP_SIZE];
    char port[MAX_PORT_LEN];
    char game_buffer[BUFFER_SIZE];
    char *data_ptr = m.data;
    int data_index = 0;
    int temp_index = 0;

    memset(name, '\0', NAME_MAX);
    memset(ip_address, '\0', IP_SIZE);
    memset(port, '\0', MAX_PORT_LEN);
    memset(game_buffer, '\0', BUFFER_SIZE);
    printf("data: %s\n", m.data);

    struct message reply;
    int k; 

    printf("m.action: %c\n", m.action);

    switch (m.action) {
        case REGISTER: // there could be an overflow error here if I'm not careful
            #ifdef DEBUG
            printf("Command - register\n");
            #endif 

            strcpy(name, get_token(m.data));
            strcpy(ip_address, get_token(NULL));
            strcpy(port, get_token(NULL));

            #ifdef DEBUG
            printf("name: %s, ip: %s, port: %s\n", name, ip_address, port);
            #endif

            register_player(name, ip_address, port);
            reply.action = REGISTER;
            reply.req = REPLY;
            reply.error_code = OK;
            reply.data_len = 3;
            strcpy(reply.data, "OK\n\0");
            struct_to_message(reply, manager_buffer);
            send_data(manager_buffer, BUFFER_SIZE);
            break;
        case QUERY_PLAYERS:
            printf("Command - query players\n");
            // REPLACE
            reply.action = QUERY_PLAYERS;
            reply.req = REPLY;
            reply.error_code = OK;
            memset(reply.data, '\0', BUFFER_SIZE);
            copy_player_info(reply.data);
            reply.data_len = strlen(reply.data);
            struct_to_message(reply, manager_buffer);
            printf("manager buffer: %s\n", manager_buffer);
            send_data(manager_buffer, BUFFER_SIZE);
            print_players();
            break;
        case START_GAME:
            // REPLACE
            printf("Command - start game\n");
            k = atoi(m.data);
            if (start_game(k, game_buffer, BUFFER_SIZE, "sue"))
            {
                reply.action = START_GAME;
                reply.req = REPLY;
                reply.error_code = OK;
                strcpy(reply.data, game_buffer);
                reply.data_len = strlen(reply.data);
                struct_to_message(reply, manager_buffer);
                printf("buffer: %s\n", manager_buffer);
                send_data(manager_buffer, BUFFER_SIZE);
                printf("Game started\n");
                // TODO - add print games
            } else
            {
                reply.action = START_GAME;
                reply.req = REPLY;
                reply.error_code = FAILURE;
                reply.data_len = 7;
                strcpy(reply.data, "FAILURE");
                struct_to_message(reply, manager_buffer);
                send_data(manager_buffer, BUFFER_SIZE);
                printf("Game not started\n");
                // TODO -- add print games
            }
            break;
        case QUERY_GAMES:
            // REPLACE
            printf("Command - query games\n");
            reply.action = QUERY_GAMES;
            reply.req = REPLY;
            reply.error_code = OK;
            copy_all_game_info(reply.data);
            reply.data_len = strlen(reply.data);
            struct_to_message(reply, manager_buffer);
            send_data(manager_buffer, BUFFER_SIZE);
            printf("command - query games\n");
            break;
        case END_GAME:
            // REPLACE
            printf("command - end game\n");
            break;
        case DEREGISTER:
            printf("Command - deregister\n");
            strcpy(name, m.data);
            deregister_player(name);
            reply.action = DEREGISTER;
            reply.req = REPLY;
            reply.error_code = OK;
            reply.data_len = 2;
            strcpy(reply.data, "OK");
            struct_to_message(reply, manager_buffer);
            send_data(manager_buffer, BUFFER_SIZE);
            break;
        default:
            // REPLACE
            printf("manger:process_command() - command not found");
            reply.action = INPUT_ERROR;
            reply.error_code = ERROR;
            strcpy(reply.data, "Command not found\n\0");
            reply.data_len = strlen(reply.data);
            struct_to_message(reply, manager_buffer);
            send_data(manager_buffer, BUFFER_SIZE);
            break;
    
    }
    #ifdef DEBUG
    printf("exiting process_command() \n");
    #endif
    return;
}

char *get_token(char *str)
{
    return strtok(str, " \0");
}

void send_data(char *data, int len)
{
    extern int socket_ID;
    write(socket_ID, data, len);
}

void copy_player_info(char *dest)
{
    char port_buffer[MAX_PORT_LEN];

    if (num_players == 0)
        strcat(dest, "No players\n");
    for (int i = 0; i < num_players; i++)
    {
        memset(port_buffer, '\0', MAX_PORT_LEN);
        sprintf(port_buffer, "%d", players[i].port);

        strcat(dest, players[i].name);
        strcat(dest, " ");
        strcat(dest, players[i].IP);
        strcat(dest, " ");
        strcat(dest, port_buffer);
        strcat(dest, "\n");
    }
    strcat(dest, "\0");

    #ifdef DEBUG
    printf("BUFFER: %s\n", dest);
    #endif
}

int start_game(int k, char * game_info, int size, char *requester) 
{
    extern struct game games[];
    extern int num_players;
    static int identifier = 1;
    extern int num_games;


    memset(game_info, '\0', size);

    if (num_players < MIN_PLAYERS + 1)
        return 0;

    if (k > num_players)
        return 0;
    
    games[num_games].id = identifier;
    games[num_games].k = k;



    // TODO: add a randomized get for the players

    // TODO: add a check for the players

    int rand_index; 
    for (int i = 0; i < k; i++)
    {
        rand_index = i;
        if (strcmp(games[num_games].players[i].name, requester) == 0)
            continue;
        strcpy(games[num_games].players[i].name, players[rand_index].name);
        strcpy(games[num_games].players[i].IP, players[rand_index].IP);
        games[num_games].players[i].port = players[rand_index].port;
    }

    
    num_games ++;
    identifier ++;
    if (copy_game_info(game_info, num_games-1))
        printf("SUCCESS\n");
    else
        printf("FAIL\n");
    return 1;
}

int copy_game_info(char *dest, int index)
{
    char id_buffer[MAX_STRING_SIZE];
    char port_buffer[MAX_PORT_LEN];

    if (num_games == 0)
    {
        return 0;
    }

    memset(id_buffer, '\0', MAX_STRING_SIZE);
    sprintf(id_buffer, "%d", games[index].id);

    strcat(dest, "Game: ");
    strcat(dest, id_buffer);
    strcat(dest, "\n");
    strcat(dest, "Players: \n");

    for (int i = 0; i < games[index].k; i++)
    {
        memset(port_buffer, '\0', MAX_PORT_LEN);
        sprintf(port_buffer, "%d", games[index].players[i].port);
        strcat(dest, games[index].players[i].name);
        strcat(dest, " ");
        strcat(dest, games[index].players[i].IP);
        strcat(dest, " ");
        strcat(dest, port_buffer);
        strcat(dest, "\n\0");
    }

    return 1;
}

void copy_all_game_info(char *dest)
{
    char id_buffer[MAX_STRING_SIZE];
    char port_buffer[MAX_PORT_LEN];

    if (num_games == 0)
    {
        strcat(dest, "No games\n");
    }
    char *temp = dest;
    
    for (int i = 0; i < num_games; i++)
    {
        if (copy_game_info(temp, i))
        {
            temp += strlen(temp);
        } else
        {
            break;
        }
        // memset(id_buffer, '\0', MAX_STRING_SIZE);
        // sprintf(id_buffer, "%d", games[i].id);

        // strcat(dest, "Game: ");
        // strcat(dest, id_buffer);
        // strcat(dest, "\n");
        // strcat(dest, "Players: \n");

        // for (int j = 0; j < games[i].k; j++)
        // {

        //     memset(port_buffer, '\0', MAX_PORT_LEN);
        //     sprintf(port_buffer, "%d", games[i].players[j].port);
        //     strcat(dest, games[i].players[j].name);
        //     strcat(dest, " ");
        //     strcat(dest, games[i].players[j].IP);
        //     strcat(dest, " ");
        //     strcat(dest, port_buffer);
        //     strcat(dest, "\n");
        // }
    }
    strcat(dest, "\0");
    #ifdef DEBUG
    printf("Buffer for game info is: \n%s\n", dest);
    #endif
}

void *
RunManager(void *arg)
{
    printf("In RunManager\n");
    ssize_t n;
    extern char manager_buffer[BUFFER_SIZE];
    extern int socket_ID;

    int sockfd = (intptr_t) arg;

    socket_ID = sockfd;


    if ( (n = read(sockfd, manager_buffer, BUFFER_SIZE)) == 0 )
        return (void *) -1; /* connection closed by other end */

    #ifdef DEBUG
    printf("manager buffer: %s\n", manager_buffer);
    #endif

    struct message t = message_to_struct(manager_buffer);
    process_command(t);
    printf("manager_buffer: %s\n", manager_buffer);
    memset(manager_buffer, '\0', BUFFER_SIZE);


    close(sockfd);
    return (void*) 1;
}


void run_server(int argc, char **argv)
{
    int sock, connfd;                /* Socket */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned int cliAddrLen;         /* Length of incoming message */
    unsigned short echoServPort;     /* Server port */
    int recvMsgSize;                 /* Size of received message */

    int num_threads = 10;
    int current_thread = 0;
    pthread_t threads[num_threads];


    echoServPort = atoi(argv[1]);  /* First arg:  local port */

    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        DieWithError("server: socket() failed");

    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */

    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("server: bind() failed");
  
	if (listen(sock, BACKLOG) < 0 )
		DieWithError("server: listen() failed");

    printf("Server running\n");

	cliAddrLen = sizeof(echoClntAddr);


    while (1)
    {
        connfd = accept( sock, (struct sockaddr *) &echoClntAddr, &cliAddrLen );

        RunManager((void*)(intptr_t)connfd);
        close(connfd);
        // void *arg = (void *) (intptr_t) connfd;

        // int error = pthread_create(&threads[current_thread], NULL, RunManager, arg);
        // if (error)
        //     printf("Error\n");
        // else
        //     printf("Thread created\n");
        
        // error = pthread_detach(threads[current_thread]);

        // if (error)
        //     printf("Failed to detach thread\n");
        // else
        //     printf("Thread detached\n");

        // current_thread++;
    }

    close(sock);
}


#ifndef TEST
int
main(int argc, char **argv)
{
    if (argc != 2)         /* Test for correct number of parameters */
    {
        fprintf(stderr,"Usage: %s <TDP SERVER PORT>\n", argv[0]);
        exit(1);
    }

    // Manager's job is to just listen for people to connect, and process their requests. 
    printf("Starting server\n");
    run_server(argc, argv);
}
#endif

#ifdef TEST
int main(int argc, char **argv)
{
    char *n1 = "Daniel";
    char *n2 = "Grant";
    char *n3 = "Sue";

    char *ip1 = "123.234.345.456";
    char *ip2 = "234.345.456.567";
    char *ip3 = "345.456.567.678";

    char *p1 = "12345";
    char *p2 = "23456";
    char *p3 = "34567";
    
    struct message mes;
    mes.action = QUERY_PLAYERS;
    mes.req = REQUEST;
    mes.error_code = OK;
    mes.data_len = 0;
    memset(mes.data, '\0', MAX_STRING_SIZE);

    register_player(n1, ip1, p1);
    register_player(n2, ip2, p2);
    register_player(n3, ip3, p3);

    //print_players();

    char buff[BUFFER_SIZE];

    int x = start_game(2, buff, BUFFER_SIZE);

    printf("x: %d\n%s\n", x, buff);

    memset(buff, '\0', BUFFER_SIZE);
    int y = start_game(3, buff, BUFFER_SIZE);
    printf("y: %d\n%s\n", y, buff);

}

#endif