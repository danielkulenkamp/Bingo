#include "peer.h"


void
str_cli(FILE *fp, int sockfd)
{
	ssize_t n;
        char    sendline[BUFFER_SIZE], recvline[BUFFER_SIZE];
        memset(sendline, '\0', BUFFER_SIZE);
        while (fgets(sendline, BUFFER_SIZE, fp) != NULL) {
            // test the register part
            struct message to_send = process_command(sendline);
            if (to_send.action == INPUT_ERROR)
                continue;

            struct_to_message(to_send, peer_buffer);

            write(sockfd, peer_buffer, strlen(peer_buffer));
                    // write(sockfd, sendline, strlen(sendline));
            if ( (n = read(sockfd, recvline, BUFFER_SIZE)) == 0)
                    DieWithError("str_cli: server terminated prematurely");

            recvline[ n ] = '\0';
            process_reply(recvline);

            memset(recvline, '\0', BUFFER_SIZE);
            memset(sendline, '\0', BUFFER_SIZE);
            memset(peer_buffer, '\0', BUFFER_SIZE);
        }
}

char *get_token(char *str)
{
    return strtok(str, " \n\0");
}

struct message process_command(char *message)
{
    #ifdef DEBUG
    printf("manager.c - process_command: Splitting first token\n");
    #endif
    char *current_token;

    current_token = get_token(message);
    
    if (current_token == NULL)
    {
        struct message err;
        err.action = INPUT_ERROR;
        err.error_code = ERROR;
        return err;
    }
    #ifdef DEBUG
    printf("Command: %s\n", current_token);
    #endif

    if (strcmp(current_token, "register") == 0)
    {
        char name[NAME_MAX];
        char ip_address[IP_SIZE];
        char port[MAX_PORT_LEN];

        memset(name, '\0', NAME_MAX);
        memset(ip_address, '\0', IP_SIZE);
        memset(port, '\0', MAX_PORT_LEN);
        
        current_token = get_token(NULL);
        if (current_token != NULL)
        {
            size_t n = strlen(current_token);
            strncpy(name, current_token, n);

            #ifdef DEBUG
            printf("name: %s\n", name);
            #endif
        }
        else
        {
            struct message err;
            err.action = INPUT_ERROR;
            err.error_code = ERROR;
            printf("Syntax error -- no name entered\n");
            return err;
        }

        current_token = get_token(NULL);
        if (current_token != NULL)
        {
            strncpy(ip_address, current_token, strlen(current_token));
            #ifdef DEBUG
            printf("IP: %s\n", ip_address);
            #endif
        }
        else
        {
            struct message err;
            err.action = INPUT_ERROR;
            err.error_code = ERROR;
            printf("Syntax error -- no IP address entered\n");
            return err;
        }

        current_token = get_token(NULL);
        if (current_token != NULL)
        {
            strncpy(port, current_token, strlen(current_token));
            #ifdef DEBUG
            printf("port: %s\n", port);
            #endif
        }
        else
        {
            struct message err;
            err.action = INPUT_ERROR;
            err.error_code = ERROR;
            printf("Syntax error -- no port entered\n");
            return err;

        }
        struct message to_send;
        to_send.action = REGISTER;
        to_send.req = REQUEST;
        to_send.error_code = OK;
        to_send.data_len = strlen(name) + strlen(ip_address) + strlen(port) + 3;

        #ifdef DEBUG
        printf("name: %s, ip: %s, port: %s,\n", name, ip_address, port);
        #endif

        int index = 0;
        for (int i = 0; i < strlen(name); i++)
            to_send.data[index++] = name[i];

        to_send.data[index++] = ' ';

        for (int i = 0; i < strlen(ip_address); i++)
            to_send.data[index++] = ip_address[i];

        to_send.data[index++] = ' ';

        for (int i = 0; i < strlen(port); i++)
            to_send.data[index++] = port[i];

        to_send.data[index] = '\0';

        #ifdef DEBUG
        printf("data: %s\n", to_send.data);
        #endif

        return to_send;

    }
    else if (strcmp(current_token, "query") == 0)
    {
        current_token = get_token(NULL);
        if (strcmp(current_token, "games") == 0)
        {
            #ifdef DEBUG
            printf("Command: query games\n");
            #endif
            struct message to_send;
            to_send.action = QUERY_GAMES;
            to_send.req = REQUEST;
            to_send.error_code = OK;
            to_send.data_len = 0;
            memset(to_send.data, '\0', MAX_STRING_SIZE);
            return to_send;
        } else if (strcmp(current_token, "players") == 0)
        {
            #ifdef DEBUG
            printf("Command: query players\n");
            #endif

            struct message to_send;
            to_send.action = QUERY_PLAYERS;
            to_send.req = REQUEST;
            to_send.error_code = OK;
            to_send.data_len = 0;
            memset(to_send.data, '\0', MAX_STRING_SIZE);

            return to_send;
        } else
        {
            struct message a;
            a.action = INPUT_ERROR;
            a.error_code = ERROR;
            printf("syntax error - not sure what to query\n");
            return a;
        }
    } 
    else if (strcmp(current_token, "deregister") == 0)
    {
        char name[NAME_MAX];


        current_token = get_token(NULL);
        if (current_token != NULL)
        {
            size_t n = strlen(current_token);
            strncpy(name, current_token, n);
            name[n] = '\0';
            struct message to_send;
            to_send.action = DEREGISTER;
            to_send.req = REQUEST;
            to_send.error_code = OK;
            to_send.data_len = strlen(name);
            strcpy(to_send.data, name);
            return to_send;

        } else
        {
            struct message err;
            err.action = INPUT_ERROR;
            err.error_code = ERROR;
            printf("ERROR -- please enter a name to deregister\n");
            return err;

            
        }

        struct message m;
        m.action = -1;
        m.error_code = ERROR;
        return m;
        
    } else if (strcmp(current_token, "start") == 0)
    {
        current_token = get_token(NULL);
        if (strcmp(current_token, "game") != 0)
        {
            printf("syntax: start game <num_players>\n");
            struct message err;
            err.action = INPUT_ERROR;
            err.error_code = ERROR;
            return err;
            
        } else
        {
            current_token = get_token(NULL);
            // verify it's a number
            for (int i = 0; i < strlen(current_token); i++)
            {
                if (!isdigit(current_token[i]))
                {
                    struct message a;
                    a.action = INPUT_ERROR;
                    a.error_code = ERROR;
                    printf("syntax error - must enter an integer\n");
                    return a;
                }
            }

            struct message to_send;
            to_send.action = START_GAME;
            to_send.req = REQUEST;
            to_send.error_code = OK;
            memset(peer_buffer, '\0', BUFFER_SIZE);
            strcat(peer_buffer, current_token);
            strcat(peer_buffer, " ");
            strcat(peer_buffer, myName);
            strcpy(to_send.data, peer_buffer);
            to_send.data_len = strlen(to_send.data);

            return to_send;
            
        }

    } else if (strcmp(current_token, "end") == 0)
    {
        current_token = get_token(NULL);
        if (strcmp(current_token, "game") != 0)
        {
            printf("syntax: end game <id>\n");
            struct message err;
            err.action = INPUT_ERROR;
            err.error_code = ERROR;
            return err;
        } else
        {
            current_token = get_token(NULL);;

            for (int i = 0; i < strlen(current_token); i++)
            {
                if (!isdigit(current_token[i]))
                {
                    struct message a;
                    a.action = INPUT_ERROR;
                    a.error_code = ERROR;
                    printf("syntax error - must enter an integer\n");
                    return a;
                }
            }

            struct message to_send;
            to_send.action = END_GAME;
            to_send.req = REQUEST;
            to_send.error_code = OK;
            strcpy(to_send.data, current_token);
            to_send.data_len = strlen(to_send.data);

            return to_send;
        }
    }
    else if (strcmp(current_token, "help") == 0)
    {
        print_help_menu();
    }  else if (strcmp(current_token, "exit") == 0)
    {
        printf("Exiting.\n");
        exit(1);
    }
    else
    {
	    printf("here unknown command\n");
        print_help_menu();
    }

	struct message a;
    a.action = INPUT_ERROR;
    return a;
	
}

int process_reply(char *reply_buffer)
{
    struct message reply = message_to_struct(reply_buffer);

    switch (reply.action)
    {
        case REGISTER:
            if (reply.error_code == OK)
            {
                printf("Player registered successfully.\n");
                return 1;
            }
            else
                printf("Error - Player not registered\n");
            break;
        case QUERY_PLAYERS:
            if (reply.error_code == OK)
            {
                printf("%s\n", reply.data);
                return 1;
            }
            else
                printf("Error - querying error\n");
            break;
        case START_GAME:
            if (reply.error_code == OK)
            {
                printf("%s\n", reply.data);
                return 1;
            }
            else
                printf("%s\n", reply.data);
            break;
        case QUERY_GAMES:
            if (reply.error_code == OK)
            {
                printf("%s\n", reply.data);
                return 1;
            }
            else
                printf("Error - querying games error\n");
            break;
        case END_GAME:
        case DEREGISTER:
            if (reply.error_code == OK)
            {
                printf("Player deregistered successfully.\n");
                return 1;
            }
            else
                printf("Error with deregister\n");
            break;
        case MAKE_CALL:
        case BINGO:
        case NOTIFY_BINGO:
        default:
            printf("unknown command. error\n");
            print_help_menu();
            break;
    }
    return 0;
}

void print_help_menu() 
{
    printf("options:\n");
    printf("register <NAME> <IP-ADDRESS> <PORT>\n");
    printf("query players\n");
    printf("start game <GAME-IDENTIFIER>\n");
    printf("query games\n");
    printf("end <GAME-IDENTIFIER>\n");
    printf("deregsiter <NAME>\n");
}

void *listen_on_new(void *arg)
{

    printf("New thread created\n");
    extern char name[];
    int sock, connfd;
    struct sockaddr_in echoServAddr;
    struct sockaddr_in echoClntAddr;
    unsigned int cliAddrLen;
    unsigned short echoServPort;

    char buf[BUFFER_SIZE];

    echoServPort = (intptr_t) arg;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        DieWithError("peer server: socket() failed\n");

    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family = AF_INET;
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    echoServAddr.sin_port = htons(echoServPort);

    if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("peer server: bind() failed");
    
    if (listen(sock, BACKLOG) < 0)
        DieWithError("peer server: listen() failed");
    
    printf("listening on port %d\n", echoServPort);

    cliAddrLen = sizeof(echoClntAddr);


    // aceept the connection only once here, because it's for a single game
    // when the game is over, this thread just terminates

    connfd = accept(sock, (struct sockaddr *) &echoClntAddr, &cliAddrLen);
    printf("received new port connection\n");


    // play game
    struct bingo_board b = generate_board();
    print_bingo_board(b);

    ssize_t n;
    while (1) // listen for connections and get new port
    {
        if ((n = read(connfd, buf, BUFFER_SIZE)) == 0)
        {
            DieWithError("listen_on_new: Server terminated early");
        }

        buf[n] = '\0';

        if (strncmp(buf, "BINGO", 5) == 0)
            break;

        printf("buffer contents: %sEND\n", buf);

        mark_call(&b, atoi(buf));
        print_bingo_board(b);

        if (check_bingo(&b))
        {
            printf("I GOT BINGO\n");
            memset(buf, '\0', BUFFER_SIZE);
            strcat(buf, "BINGO ");
            strcat(buf, name);
            n = write(connfd, buf, strlen(buf));
            printf("bytes written: %ld\n", n);
            printf("error: %s\n", strerror(errno));
            break;
        }
    }

    char *temp;
    temp = strtok(buf, " \n");
    temp = strtok(NULL, " \n");
    printf("%s GOT BINGO\n", temp);

    close(connfd);

    return (void*)(intptr_t) 1;

}

void *listen_on_default(void *arg)
{
    extern int basePort;
    int default_port = (intptr_t) arg;
    int sock, connfd;
    struct sockaddr_in echoServAddr;
    struct sockaddr_in echoClntAddr;
    unsigned int cliAddrLen;
    unsigned short echoServPort;

    char buf[MAX_STRING_SIZE];

    echoServPort = default_port;
    basePort = default_port + 1;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        DieWithError("peer server: socket() failed\n");

    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family = AF_INET;
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    echoServAddr.sin_port = htons(echoServPort);

    if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("peer server: bind() failed");
    
    if (listen(sock, BACKLOG) < 0)
        DieWithError("peer server: listen() failed");
    printf("default port listening\n");

    cliAddrLen = sizeof(echoClntAddr);

    pthread_t thread;

    while (1) // listen for connections and get new port
    {
        connfd = accept(sock, (struct sockaddr *) &echoClntAddr, &cliAddrLen);

        memset(buf, '\0', MAX_STRING_SIZE);
        sprintf(buf, "%d", basePort);
        printf("received connection\n");


        write(connfd, buf, strlen(buf));

        close(connfd);

        pthread_create(&thread, NULL, listen_on_new, (void*)(intptr_t) basePort);

        //pthread_join(thread, NULL);

        basePort += 1;


    }
}


int count_players(char *player_list)
{
    int count = 0;
    char *str;
    str = strtok(player_list, "\n");
    while (str != NULL)
    {
        count ++;
        str = strtok(NULL, "\n");
    }
    return count;
}


int Register(char *name, char *ip, char *port)
{
	int sockfd;
	struct sockaddr_in servaddr;

	extern char managerIP[];
    extern char manager_port[];

    char sendline[BUFFER_SIZE], recvline[BUFFER_SIZE];
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
	// bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(manager_port));
	inet_pton(AF_INET, managerIP, &servaddr.sin_addr);

	connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    memset(sendline, '\0', BUFFER_SIZE);
    memset(recvline, '\0', BUFFER_SIZE);
    ssize_t n;

    strcat(sendline, "register ");
    strcat(sendline, name);
    strcat(sendline, " ");
    strcat(sendline, ip);
    strcat(sendline, " ");
    strcat(sendline, port);

    struct message to_send = process_command(sendline);
    if (to_send.action == INPUT_ERROR)
        DieWithError("Register: input error");

    struct_to_message(to_send, peer_buffer);

    write(sockfd, peer_buffer, strlen(peer_buffer));

    if ( (n = read(sockfd, recvline, BUFFER_SIZE)) == 0)
        DieWithError("Register: server terminated prematurely");

    recvline[n] = '\0';
    int result = process_reply(recvline);
    if (!result)
    {
        printf("Error adding player. Name already registered\n");
    }

    memset(recvline, '\0', BUFFER_SIZE);
    memset(sendline, '\0', BUFFER_SIZE);
    memset(peer_buffer, '\0', BUFFER_SIZE);

    return result;

}

int DeRegister(char *name)
{
    int sockfd;
    struct sockaddr_in servaddr;

    extern char managerIP[];
    extern char manager_port[];

    char sendline[BUFFER_SIZE], recvline[BUFFER_SIZE];
    ssize_t n;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(manager_port));
    inet_pton(AF_INET, managerIP, &servaddr.sin_addr);

    connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    memset(sendline, '\0', BUFFER_SIZE);
    memset(recvline, '\0', BUFFER_SIZE);

    strcat(sendline, "deregister ");
    strcat(sendline, name);

    struct message to_send = process_command(sendline);

    if (to_send.action == INPUT_ERROR)
        DieWithError("Deregister: input error");

    struct_to_message(to_send, peer_buffer);

    write(sockfd, peer_buffer, strlen(peer_buffer));

    if ( (n = read(sockfd, recvline, BUFFER_SIZE)) == 0)
        DieWithError("DeRegister: server terminated prematurely");

    recvline[n] = '\0';
    int result = process_reply(recvline);

    memset(recvline, '\0', BUFFER_SIZE);
    memset(sendline, '\0', BUFFER_SIZE);
    memset(peer_buffer, '\0', BUFFER_SIZE);

    return result;
}
int QueryPlayers()
{
    int sockfd;
	struct sockaddr_in servaddr;

	extern char managerIP[];
    extern char manager_port[];

    char sendline[BUFFER_SIZE], recvline[BUFFER_SIZE];
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
	// bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(manager_port));
	inet_pton(AF_INET, managerIP, &servaddr.sin_addr);

	connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    memset(sendline, '\0', BUFFER_SIZE);
    memset(recvline, '\0', BUFFER_SIZE);
    ssize_t n;

    strcat(sendline, "query players");

    struct message to_send = process_command(sendline);
    if (to_send.action == INPUT_ERROR)
        DieWithError("QueryPlayers: input error");

    struct_to_message(to_send, peer_buffer);

    write(sockfd, peer_buffer, strlen(peer_buffer));

    if ( (n = read(sockfd, recvline, BUFFER_SIZE)) == 0)
        DieWithError("Register: server terminated prematurely");
    recvline[n] = '\0';
    printf("%s\n", recvline);
    int result = process_reply(recvline);
    if (!result)
    {
        printf("Num players: %d\n", 0);
        return 0;
    }
    int count = count_players(recvline);
    printf("Num players: %d\n", count);

    return count;

}

int QueryGames()
{
    int sockfd;
    struct sockaddr_in servaddr;

    extern char managerIP[];
    extern char manager_port[];

    char sendline[BUFFER_SIZE], recvline[BUFFER_SIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(manager_port));
    inet_pton(AF_INET, managerIP, &servaddr.sin_addr);

    connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    memset(sendline, '\0', BUFFER_SIZE);
    memset(recvline, '\0', BUFFER_SIZE);
    ssize_t n;

    strcat(sendline, "query games");
    
    struct message to_send = process_command(sendline);
    if (to_send.action == INPUT_ERROR)
        DieWithError("QueryGames: input error");
    
    struct_to_message(to_send, peer_buffer);

    write(sockfd, peer_buffer, strlen(peer_buffer));

    if ( (n = read(sockfd, recvline, BUFFER_SIZE)) == 0)
        DieWithError("QueryGames: server terminated prematurely");
    recvline[n] = '\0';
    printf("%s\n", recvline);
    int result = process_reply(recvline);
    
    if (!result)
        return 0;
    else 
        return 1;
}
int StartGameWithManager(int num_players)
{
    extern char managerIP[];
    extern char manager_port[];
    extern char name[];
    extern char myName[];

    int sockfd;
    struct sockaddr_in servaddr;

    char sendline[BUFFER_SIZE], recvline[BUFFER_SIZE];
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(manager_port));
    inet_pton(AF_INET, managerIP, &servaddr.sin_addr);

    connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    memset(sendline, '\0', BUFFER_SIZE);
    memset(recvline, '\0', BUFFER_SIZE);
    ssize_t n;
    
    char temp[10];
    sprintf(temp, "%d", num_players);
    strcat(sendline, "start game ");
    strcat(sendline, temp);
    memset(name, '\0', NAME_MAX);
    sprintf(name, "%s", myName);
    strcat(sendline, " ");
    strcat(sendline, name);

    struct message to_send = process_command(sendline);
    if (to_send.action == INPUT_ERROR)
        DieWithError("StartGameWithManager: input error");
    
    struct_to_message(to_send, peer_buffer);

    write(sockfd, peer_buffer, strlen(peer_buffer));

    if ( (n = read(sockfd, recvline, BUFFER_SIZE)) == 0)
        DieWithError("StartGameWithManager: read error");

    recvline[n] = '\0';

    struct message result = message_to_struct(recvline);

    if (result.error_code != OK)
        return 0;


    
    printf("StartGameWithManager: buffer - %s\n", recvline);


    int x = save_game_info(recvline);
    printf("Exiting StartGameWithManager\n");

    return x;
}

int EndGameWithManager(int id)
{
    extern char managerIP[];
    extern char manager_port[];

    int sockfd;
    struct sockaddr_in servaddr;

    char sendline[BUFFER_SIZE], recvline[BUFFER_SIZE];
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(manager_port));
    inet_pton(AF_INET, managerIP, &servaddr.sin_addr);

    connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    memset(sendline, '\0', BUFFER_SIZE);
    memset(recvline, '\0', BUFFER_SIZE);
    ssize_t n;

    char temp[10];
    sprintf(temp, "%d\n", id);
    strcat(sendline, "end game ");
    strcat(sendline, temp);

    printf("sendline: %s\n", sendline);

    struct message to_send = process_command(sendline);
    if (to_send.action == INPUT_ERROR)
        DieWithError("EndGameWithManager: input error");

    struct_to_message(to_send, peer_buffer);

    write(sockfd, peer_buffer, strlen(peer_buffer));

    if ( (n = read(sockfd, recvline, BUFFER_SIZE)) == 0)
        DieWithError("EndGameWithManager: read error");

    recvline[n] = '\0';

    struct message result = message_to_struct(recvline);

    if (result.error_code != OK)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int save_game_info(char *info)
{
    extern struct game current_game;
    char *temp;
    int player_index = 0;
    int id; 

    temp = strtok(info, " \n"); // 2|2|A|ID 
    printf("After first: %s\n", temp);
    if (temp[4] == FAILURE)
    {
        printf("Game failed to start. \n");
        return 0;
    }
    temp = strtok(NULL, " \n"); // game identifier
    printf("After second: %s\n", temp);
    current_game.id = atoi(temp);
    id = current_game.id;

    memset(current_game.players, '\0', sizeof(struct player) * MAX_PLAYERS);

    temp = strtok(NULL, " \n"); // Players: 
    printf("After third: %s\n", temp);
    temp = strtok(NULL, " \n"); // gets the first player
    while (temp != NULL)
    {
        strcpy(current_game.players[player_index].name, temp);
        temp = strtok(NULL, " \n"); // moves to the IP address
        printf("After first in while: %s\n", temp);
        strcpy(current_game.players[player_index].IP, temp);
        temp = strtok(NULL, " \n"); // moves to the port
        printf("After second in while: %s\n", temp);
        current_game.players[player_index].port = atoi(temp);

        player_index ++; 
        temp = strtok(NULL, " \n"); // moves index to next player, or sets temp to NULL if out of players
        printf("After last in while: %s\n", temp);
    }

    current_game.k = player_index;
    return id;
}

/* 
 * CALLER RELATED FUNCTIONS
 */

void SetupBingo()
{
    bingo_bag = bingo_bag_init();
}

void TearDownBingo()
{
    bag_deinit(bingo_bag);
}

void InitiateTCPConnections()
{
    extern struct game current_game;
    // extern char player_buffers[][BUFFER_SIZE];
    extern int player_socks[];
    int temp_socket;
    ssize_t n;
    struct sockaddr_in servaddr;

    char recvline[BUFFER_SIZE];

    for (int i = 0; i < current_game.k; i++)
    {
        // set sockadd_in fields for each player
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(current_game.players[i].port);
        inet_pton(AF_INET, current_game.players[i].IP, &servaddr.sin_addr);

        // create a temp socket for the default port
        temp_socket = socket(AF_INET, SOCK_STREAM, 0);

        // connect to the player
        if (connect(temp_socket, (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0)
            DieWithError("InitiateTCPConnections: error connecting player's default port");

        // read new port from player
        if ( (n = read(temp_socket, recvline, BUFFER_SIZE)) == 0)
            DieWithError("InitiateTCPConnections: couldn't read the port");
        recvline[n] = '\0';
        printf("new port: %s\n", recvline);

        close(temp_socket);
        sleep(3);
        // now we have the new port number, so we initiate the new ones

        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(atoi(recvline));
        inet_pton(AF_INET, current_game.players[i].IP, &servaddr.sin_addr);

        // create new socket just for this player
        player_socks[i] = socket(AF_INET, SOCK_STREAM, 0);
        printf("Attempting to connect to new port %s\n", recvline);
        if (connect(player_socks[i], (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0)
            DieWithError("InitiateTCPConnctions: error connecting to player's new port");

        // setup poll
        ufds[i].fd = player_socks[i];
        ufds[i].events = POLLIN;

        // here you are done, since we are just initiating
    }
}

void CloseTCPConnections()
{
    extern int player_socks[];
    for (int i = 0; i < current_game.k; i++)
    {
        close(player_socks[i]);
    }
}

void MakeCall()
{
    printf("MakeCall entered\n");
    extern int player_socks[];
    char send_buffer[32];

    int current_call = call(bingo_bag);
    memset(send_buffer, '\0', 32);
    sprintf(send_buffer, "%d", current_call);

    for (int i = 0; i < current_game.k; i++)
    {
        if (write(player_socks[i], send_buffer, strlen(send_buffer)) <= 0)
            DieWithError("MakeCall: can't write to socket");
    }
    printf("MakeCall Exiting\n");
}

void SendBingo(char *winner)
{
    printf("SendBingo entered\n");
    extern int player_socks[];
    char send_buffer[32];

    // int current_call = call(bingo_bag);
    memset(send_buffer, '\0', 32);
    strcpy(send_buffer, "BINGO ");
    strcat(send_buffer, winner);

    for (int i = 0; i < current_game.k; i++)
    {
        if (write(player_socks[i], send_buffer, strlen(send_buffer)) <= 0)
            DieWithError("MakeCall: can't write to socket");
    }
    printf("SendBingo exited\n");
}

bool CheckWinner()
{
    // read from each socket and see if there is a bingo
    // if there is, send that out to everyone, return true;
    // else, return false
    printf("CheckWinner entered\n");
    extern int player_socks[];
    extern struct pollfd ufds[];
    extern struct game current_game;

    char buffer[BUFFER_SIZE];
    char *temp;
    ssize_t n;
    bool winner = false;

    int rv;

    // DOUBLE CHECK, there might be an error here because the udfs was misspelled
    rv = poll(ufds, current_game.k, 1000);

    if (rv == -1)
    {
        DieWithError("CheckWinner - poll error");
    } else if (rv == 0)
    {
        return false;
    } else
    {
        for (int i = 0; i < current_game.k; i++)
        {
            if (ufds[i].revents & POLLIN)
            {
                n = read(player_socks[i], buffer, BUFFER_SIZE);
                if (n == 0)
                {
                    printf("i = %d\n", i);
                    //DieWithError("CheckWinner - should have read data but didn't");
                }
                else 
                {
                    buffer[n] = '\0';
                    temp = strtok(buffer, " \n");
                    winner = true;
                    printf("CheckWinner - temp is %s\n", temp);
                    if (strcmp(temp, "BINGO") == 0)
                    {
                        // we have a bingo 
                        // get the name of the person who won
                        temp = strtok(NULL, " \n");
                        break;
                    } else
                    {
                        DieWithError("CheckWinner - something went wrong");
                    }
                }

            }
        }
        printf("CheckWinner after for loop\n");

        if (!winner)
            return false;
        else 
        {   
            printf("Person who won is: %s\n", temp);
            SendBingo(temp); 
            return true;
        }

    }

// this could be an error
    return true;
}

void *Caller(void *arg)
{
    printf("Bingo called\n");
    int num_players;

    num_players = QueryPlayers();

    while (num_players < 2)
    {
        sleep(1);
        num_players = QueryPlayers();
        printf("num_players = %d\n", num_players);
    }

    int n_players = (intptr_t) arg;

    StartGameWithManager(n_players);

    printf("main: Returned from StartGameWithManager\n");

    InitiateTCPConnections();
    SetupBingo();

    while (1)
    {
        MakeCall();
        //sleep(1);
        if (CheckWinner())
            break;
    }

    TearDownBingo();
    CloseTCPConnections();

    return (void*)(intptr_t) 1;
}

bool verifyIP(char *ip)
{
    char copy[IP_SIZE];
    char *temp;
    strcpy(copy, ip);


    for (int count = 0; count < 4; count++)
    {
        if (count == 0)
        {
            temp = strtok(copy, ".\n");
        } else 
        {
            temp = strtok(NULL, ".\n");
        }

        
        for (int i = 0; i < strlen(temp); i++)
            if (!isdigit(temp[i]))
                return false;

        if (!(atoi(temp) >= 0 && atoi(temp) < 256))
            return false;

    }

    if (strlen(ip) > IP_SIZE && ip[IP_SIZE-1] != '\0')
        return false;


    return true;
    
    
}

bool verifyPort(char *port)
{
    for (int i = 0; i < strlen(port); i++)
    {
        if (!(isdigit(port[i]) || port[i] == '\0' || port[i] == '\n'))
            return false;
    }
    if (atoi(port) < 20000 || atoi(port) > 20999)
        return false;

    return true;
}

#ifdef FINAL
int main(int argc, char **argv)
{
    extern char name[];
    extern char IP[];
    extern char port[];
    extern int basePort;
    extern char managerIP[];
    extern char manager_port[];

    extern char myName[];
    extern bool myNameSet;
    myNameSet = false;

    pthread_t listening_thread;
    pthread_t bingo_thread;

    char main_buffer[BUFFER_SIZE], *temp;

    memset(name, '\0', NAME_MAX);
    memset(IP, '\0', IP_SIZE);
    memset(port, '\0', MAX_PORT_LEN);

    int looping = 1;

    if (argc < 3)
    {
        printf("usage: ./peer <managerIP> <managerPort>\n");
        exit(0);
    } else
    {
        strcpy(managerIP, argv[1]);
        strcpy(manager_port, argv[2]);
        if (!verifyIP(managerIP))
        {
            printf("IP verify failed\n");
            printf("usage ./peer <managerIP> <managerPort>\n");
            exit(0);
        }

        if (!verifyPort(manager_port))
        {
            printf("port %s", manager_port);
            printf("port verify failed\n");
            printf("ports must be within 20000-20999\n");
            printf("usage ./peer <managerIP> <managerPort>\n");
            exit(0);
        }
    }

    while (looping)
    {
        memset(main_buffer, '\0', BUFFER_SIZE);
        if (fgets(main_buffer, BUFFER_SIZE, stdin) == NULL)
        {
            printf("main: fgets error\n");
        }
        else
        {
            if (main_buffer[0] == '\n')
            {
                continue;
            }
            
            //printf("main buffer: %s\n", main_buffer);
            temp = strtok(main_buffer, " ");
            
            if (strcmp(temp, "register") == 0)
            {
                printf("Register\n");

                if (myNameSet)
                {
                    printf("you are already registered. Please unregister before registering again. \n");
                    continue;
                }
                temp = strtok(NULL, " ");

                if (temp)
                {
                    strcpy(name, temp);
                    printf("a\n");
                }
                else
                {
                    printf("a usage: register <name> <ip address> <port> \n");
                    continue;
                }

                temp = strtok(NULL, " ");
                if (temp)
                {
                    strcpy(IP, temp);
                    printf("b\n");
                }
                else
                {
                    printf("b usage: register <name> <ip address> <port> \n");
                    continue;
                }
                
                temp = strtok(NULL, " \n");
                if (temp)
                {
                    strcpy(port, temp);
                    printf("c\n");
                }
                else
                {
                    printf("name: %s\nip: %s\ntemp: %s", name, IP, temp);
                    printf("usage: register <name> <ip address> <port> \n");
                    continue;
                }

                if (!verifyIP(IP))
                {
                    printf("Error - IP address out of range\n");
                    printf("usage: register <name> <ip address> <port> \n");
                    continue;

                }

                if (!verifyPort(port))
                {
                    printf("Error - port not correct\n");
                    printf("usage: register <name> <ip address> <port> \n");
                    continue;
                }

                // Register
                if (Register(name, IP, port))
                {
                    pthread_create(&listening_thread, NULL, *listen_on_default, (void*)(intptr_t) atoi(port));
                    if (pthread_detach(listening_thread))
                        DieWithError("failed to detach thread");

                    printf("Listening on default port. \n");
                    strcpy(myName, name);
                    myNameSet = true;
                }
            } else if (strcmp(temp, "query") == 0)
            {
                printf("Query\n");
                temp = strtok(NULL, " \n");
                if (strcmp(temp, "players") == 0)
                {
                    QueryPlayers();
                } else if (strcmp(temp, "games") == 0)
                {
                    QueryGames();
                } else 
                {
                    printf("Input error\n");
                    printf("Usage: query <players | games>\n");
                }
            } else if (strcmp(temp, "start") == 0)
            {
                printf("Start\n");
                temp = strtok(NULL, " \n");
                if (strcmp(temp, "game") != 0)
                {
                    printf("Usage: start game <k>\n");
                    continue;
                }
                temp = strtok(NULL, " \n");

                for (int i = 0; i < strlen(temp); i++)
                {
                    if (!(isdigit(temp[i])))
                    {
                        printf("usage: start game <k>\n");
                        continue;
                    }
                }

                int k = atoi(temp);
                int id;
                if ((id = StartGameWithManager(k)))
                {
                    printf("Game successfully started\n");
                    pthread_create(&bingo_thread, NULL, &Caller, (void*) (intptr_t)k);
                    pthread_join(bingo_thread, NULL);
                    // TODO Deregister game
                    EndGameWithManager(id);
                }
                else
                    printf("Error starting game. Please try again. \n");
                
            } else if (strcmp(temp, "end") == 0)
            {
                printf("End\n");
                temp = strtok(NULL, " \n");
                if (strcmp(temp, "game") == 0)
                {
                    temp = strtok(NULL, " \n");
                    int id = atoi(temp);
                    if (EndGameWithManager(id))
                    {
                        printf("Game successfully ended.\n");
                        continue;
                    } else
                    {
                        printf("Error ending game.\n");
                        continue;
                    }
                } else
                {
                    printf("usage: end game <k>\n");
                    continue;
                }
            } else if (strcmp(temp, "deregister") == 0)
            {
                printf("Deregister\n");
                temp = strtok(NULL, " \n");

                if (temp)
                {
                    strcpy(name, temp);
                    printf("z\n");
                } else
                {
                    printf("usage: deregister <name>\n");
                    continue;
                }

                if (DeRegister(temp))
                {
                    printf("Successfully deregistered user %s\n", temp);
                    myNameSet = false;
                }
            } else if (strcmp(temp, "exit\n") == 0)
            {
                exit(0);
            } else
            {
                printf("unknown command\n");
                print_help_menu();
            }
        }
    }
}
#endif

#ifdef TEST2
int
main(int argc, char **argv)
{
    // What the peer needs to do

    // Register with the manager
        // upon register:
            // start listening thread on default port
    // query players, to see if there are more than 1 registered
    // if so, start a game with a random number
        // initiate connections with each player, getting new ports. 
        // initiate connections with each player on the new ports
        // start making calls every 2 seconds
        // send calls to players
    // else, sleep and then try again
    

    extern char name[];
    extern char IP[];
    extern char port[];
    extern int basePort;

    memset(name, '\0', NAME_MAX);
    memset(IP, '\0', IP_SIZE);
    memset(port, '\0', MAX_PORT_LEN);

    if (argc == 2)
    {
        basePort = 22222;
        strcat(name, "daniel\n");
        strcat(IP, "127.0.0.1\n");
        strcat(port, "19000\n");

    } else if (argc == 3)
    {
        basePort = 33333;
        strcat(name, "sue\n");
        strcat(IP, "127.0.0.1\n");
        strcat(port, "19001\n");
    } else if (argc == 4)
    {
        basePort = 55555;
        strcat(name, "david\n");
        strcat(IP, "127.0.0.1\n");
        strcat(port, "19003");
    } else if (argc == 5)
    {
        basePort = 66666;
        strcat(name, "cindy\n");
        strcat(IP, "127.0.0.1\n");
        strcat(port, "19004");
    }
    else
    {
        basePort = 44444;
        strcat(name, "grant\n");
        strcat(IP, "127.0.0.1\n");
        strcat(port, "19002\n");
    }


    // printf("Please enter your name: \n");
    // fgets(name, NAME_MAX, stdin);
    // printf("Please enter your IP address\n");
    // fgets(IP, IP_SIZE, stdin);
    // printf("Please enter your default port\n");
    // fgets(port, MAX_PORT_LEN, stdin);

    name[strcspn(name, "\n")] = '\0';
    IP[strcspn(IP, "\n")] = '\0';
    port[strcspn(port, "\n")] = '\0';

    printf("Attempting to register (%s, %s, %s)\n", name, IP, port);
    if (Register(name, IP, port))
        printf("Successfully registered.\n");

    pthread_t listening_thread;

    pthread_create(&listening_thread, NULL, &listen_on_default, (void*)(intptr_t) atoi(port));

    if (pthread_detach(listening_thread))
        DieWithError("failed to detach thread\n");

    printf("Listening on default port.\n");

    printf("Checking Players\n");

    sleep(2);
    QueryPlayers();

    sleep(2);

    printf("Attempting to Start Game\n");

    // call bingo

    pthread_t bingo_thread;
    if (argc == 2)
    {
        pthread_create(&bingo_thread, NULL, &Caller, (void*) 2);
        pthread_join(bingo_thread, NULL);

    } else if (argc == 5)
    {
        pthread_create(&bingo_thread, NULL, &Caller, (void*) 3);
        pthread_join(bingo_thread, NULL);
    }


    // here you need to update the manager that the game ended
    printf("Bingo completed\nExiting...\n");
    // before you exit you need to deregister the player


    fgets(name, NAME_MAX, stdin);
}
#endif

#ifdef TEST
int main(int argc, char **argv)
{
    char test[] = "grant 123.123.123.123 123\ndaniel 321.321.321.321 321\n\0";

    int count = count_players(test);

    printf("Players = %d\n", count);
}

#endif
