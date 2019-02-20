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
            strcpy(to_send.data, current_token);
            to_send.data_len = strlen(to_send.data);

            return to_send;
            
        }

    } else if (strcmp(current_token, "help") == 0)
    {
        print_help_menu();
    }  else if (strcmp(current_token, "exit") == 0)
    {
        printf("Exiting.\n");
        exit(1);
    }
    else
    {
	    printf("unknown command\n");
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
    int recvMsgSize;

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

    int max = 10;
    int count = 0;

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
            printf("bytes written: %d\n", n);
            printf("error: %s\n", strerror(errno));
            break;
        }
    }

    printf("SOMEONE GOT BINGO\n");
    // TODO - notify caller of bingo

    close(connfd);

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
    int recvMsgSize;

    char buf[MAX_STRING_SIZE];

    echoServPort = default_port;

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
        sprintf(buf, "%d\0", basePort);
        printf("received connection\n");


        write(connfd, buf, strlen(buf));

        close(connfd);

        pthread_create(&thread, NULL, listen_on_new, (void*)(intptr_t) basePort);

        //pthread_join(thread, NULL);

        basePort += 1;


    }
}

void try_to_connect(int port)
{
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);

    connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    char buf[MAX_STRING_SIZE];
    memset(buf, '\0', MAX_STRING_SIZE);

    strcat(buf, "get port");

    write(sockfd, buf, strlen(buf));

    int n;
    char recvline[MAX_STRING_SIZE];

    if ((n = read(sockfd, recvline, BUFFER_SIZE)) == 0)
        DieWithError("read failed");

    recvline[n] = '\0';

    printf("result: %s\n", recvline);

    int new_port = atoi(recvline);

    close(sockfd);

    sleep(2);
    // make connection to new one
    printf("HERE\n");

    int new_sockfd;
    new_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(new_port);
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);


    if (connect(new_sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
        DieWithError("connection failed\n");
    printf("HEARE\n");
    memset(buf, '\0', MAX_STRING_SIZE);
    strcat(buf, "HIIII");

    write(new_sockfd, buf, strlen(buf));
    printf("HEEEEE\n");
    if ((n = read(new_sockfd, recvline, BUFFER_SIZE)) == 0)
        DieWithError("second read failed");

    recvline[n] = '\0';
    printf("second result: %s\n", recvline);

    close(new_sockfd);




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

	char managerIP[] = "127.0.0.1";
    char manager_port[] = "10000";

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

    memset(recvline, '\0', BUFFER_SIZE);
    memset(sendline, '\0', BUFFER_SIZE);
    memset(peer_buffer, '\0', BUFFER_SIZE);

    return result;

}

int QueryPlayers()
{
    int sockfd;
	struct sockaddr_in servaddr;

	char managerIP[] = "127.0.0.1";
    char manager_port[] = "10000";

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
        DieWithError("Register: input error");

    struct_to_message(to_send, peer_buffer);

    write(sockfd, peer_buffer, strlen(peer_buffer));

    if ( (n = read(sockfd, recvline, BUFFER_SIZE)) == 0)
        DieWithError("Register: server terminated prematurely");
    recvline[n] = '\0';
    printf("%s\n", recvline);
    int result = process_reply(recvline);
    int count = count_players(recvline);
    printf("Num players: %d\n", count);

    return count;

}

int StartGameWithManager(int num_players)
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
    sprintf(temp, "%d\0", num_players);
    strcat(sendline, "start game ");
    strcat(sendline, temp);

    struct message to_send = process_command(sendline);
    if (to_send.action == INPUT_ERROR)
        DieWithError("StartGameWithManager: input error");
    
    struct_to_message(to_send, peer_buffer);

    write(sockfd, peer_buffer, strlen(peer_buffer));

    if ( (n = read(sockfd, recvline, BUFFER_SIZE)) == 0)
        DieWithError("StartGameWithManager: read error");

    recvline[n] = '\0';
    printf("StartGameWithManager: buffer - %s\n", recvline);


    save_game_info(recvline);
    printf("Exiting StartGameWithManager\n");

    return 1;
}

void save_game_info(char *info)
{
    extern struct game current_game;
    char *temp;
    int player_index = 0;

    temp = strtok(info, " \n"); // 2|2|A|Game: 
    printf("After first: %s\n", temp);
    temp = strtok(NULL, " \n"); // game identifier
    printf("After second: %s\n", temp);
    current_game.id = atoi(temp);

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
    extern char player_buffers[][BUFFER_SIZE];
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

    int current_call = call(bingo_bag);
    memset(send_buffer, '\0', 32);
    strcpy(send_buffer, "BINGO");

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
    extern struct pollfd udfs[];
    extern struct game current_game;

    char buffer[BUFFER_SIZE];
    char *temp;
    ssize_t n;
    bool winner = false;

    int rv;

    rv = poll(ufds, current_game.k, 100);

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
            SendBingo(temp); 
            return true;
        }

    }
}

void *Caller(void *arg)
{
    printf("Bingo called\n");
    // query players and save result
    // convert string of players to individual players
    // create TCP connections to all of them
    // start the bingo game
    // start sending calls, and listening for responses
    // if you get a response, send the event to all peers, close the connections
    // return


    // TODO the manager needs to check the client's identity, so that it doesn't 
    // give the same players back. 

    int num_players;

    num_players = QueryPlayers();

    while (num_players < 2)
    {
        sleep(2);
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

    // get new ports
    // start new connections











    // int defaultSocket, newSocket, managerSocket;
	// struct sockaddr_in servaddr;

    // char sendline[BUFFER_SIZE], recvline[BUFFER_SIZE], game_info[BUFFER_SIZE];

    // extern char managerIP[];
    // extern char manager_port[];


    // ssize_t n;

    // managerSocket = socket(AF_INET, SOCK_STREAM, 0);

    // memset(&servaddr, 0, sizeof(servaddr));
    // servaddr.sin_family = AF_INET;
    // servaddr.sin_port = htons(atoi(manager_port));
    // inet_pton(AF_INET, managerIP, &servaddr.sin_addr);


    // // TODO - add a way to get different amounts of players in each game
    // // Start game


    // managerSocket = socket(AF_INET, SOCK_STREAM, 0);

    // int managerConnfd = connect(managerSocket, (struct sockaddr *) &servaddr, sizeof(servaddr));
    // if (managerConnfd != 0)
    //     DieWithError("CallBingo");

    
    // sleep(1);
    
    // memset(sendline, '\0', BUFFER_SIZE);
    // memset(recvline, '\0', BUFFER_SIZE);

    // strcat(sendline, "start game 2");
    // struct message to_send = process_command(sendline);

    // if (to_send.action == INPUT_ERROR)
    //     DieWithError("Register: input error");

    // struct_to_message(to_send, peer_buffer);

    // write(managerConnfd, peer_buffer, strlen(peer_buffer));
    // memset(recvline, 0, BUFFER_SIZE);

    // if ( (n = read(managerConnfd, recvline, BUFFER_SIZE)) == 0)
    //     DieWithError("Register: server terminated prematurely");

    // printf("%s\n", recvline);
    // // TODO: Start here. It looks like up to here we are good or almost good. 
    // // receive line is just showing what we sent? so the manager is just repeating it back? 
    // // either way, it's not correct

    // int process_reply_result = process_reply(recvline);

    // printf("Game info: %s\n", game_info);




    // // Below here is where we connect to the peer
	
	// defaultSocket = socket(AF_INET, SOCK_STREAM, 0);

    // memset(&servaddr, 0, sizeof(servaddr));
	// // bzero(&servaddr, sizeof(servaddr));
	// servaddr.sin_family = AF_INET;
	// servaddr.sin_port = htons(atoi(manager_port));
	// inet_pton(AF_INET, managerIP, &servaddr.sin_addr);

	// int result = connect(defaultSocket, (struct sockaddr *) &servaddr, sizeof(servaddr));

    // if (result == 0)
    //     printf("The connection worked!\n");

    // // read from socket to get the new port number
    // if ( (n=read(defaultSocket, recvline, BUFFER_SIZE)) == 0)
    //     DieWithError("CallBingo 1 - server terminated early\n");

    // recvline[n] = '\0';

    // int newPort = atoi(recvline);

    // printf("newport: %d\n|", newPort);

    // close(defaultSocket);

    // newSocket = socket(AF_INET, SOCK_STREAM, 0);
    // memset(&servaddr, 0, sizeof(servaddr));
    // servaddr.sin_family = AF_INET;
    // servaddr.sin_port = htons(newPort);
    // inet_pton(AF_INET, managerIP, &servaddr.sin_addr);

    // int newPortResult = connect(newSocket, (struct sockaddr *) &servaddr, sizeof(servaddr));

    // if (newPortResult == 0)
    //     printf("The new port connection worked!\n");
    // else
    //     printf("It failed\n");


    // struct bag *bingo_bag;
    // bingo_bag = bingo_bag_init();

    // // make calls every second and send to players
    // int current_call;
    // while (1)
    // {
    //     current_call = call(bingo_bag);
    //     memset(sendline, 0, BUFFER_SIZE);
    //     sprintf(sendline, "%d", current_call);
    //     if (write(newSocket, sendline, strlen(sendline)) <= 0)
    //         break;
    //     sleep(1);
    //     // TODO - need to poll each socket, and check if they have a bingo
    //     // if they do, send out a new response saying that there was a bingo
    //     // print the winner, then close all connections. 
    // }

    // bag_deinit(bingo_bag);
    

    // int bytes_written = write(newSocket, managerIP, strlen(managerIP));

    // printf("bytes written: %d|\n", bytes_written);

    // close(newSocket);
}

#ifndef TEST
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
