#ifndef SERVER_H
# define SERVER_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <sys/socket.h>
# include <arpa/inet.h>

# define BUF_SIZE 1024
# define MAX_CLNT 256

typedef enum e_order {
    EXIT = 0,
    LOBBY,
    HELP,
    WHISPER,
    ROOM,
    CREATE,
    JOIN,
    LEAVE,
    LIST,
    WALL,
    GAME,
    IGNORE
}   t_order;

typedef struct s_map {
    char *key;
    int value;
}   t_map;

typedef struct s_server {
    int serv_sock;
    int clnt_sock;
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;

    int clnt_cnt;
    int clnt_socks[MAX_CLNT];
    int clnt_addr_size;

    t_map *list;
}   t_server;

typedef struct s_mutex_list {
    pthread_mutex_t *s_a;
    pthread_mutex_t *map;
}   t_mutex_list;

typedef struct s_connected {
    int clnt_sock;
    int clnt_cnt;
    char name[20];

    t_server *server;
    t_mutex_list *mutex_list;
    pthread_t *t_id;
}   t_connected;

//
// init.c
//
t_mutex_list *init_mutex();
t_server *init_server(char **argv);

//
// server.c
//
void run_server(t_server *server, t_mutex_list *mutex_list);

//
// client_handler.c
//
void *handle_clnt(void *arg);

//
// operations
//
void whisper(t_connected *client, char *text);

//
// utils.c
//
int parser(char *msg, char **text);
int find_order(char *order);
void add_name_list(t_connected *client);
void delete_name_list(t_connected *client);
int dup_check(t_connected *client);
void send_all(t_connected *client, char *msg);
void send_all_j(t_connected *client);
char **ft_split(char *s, char c);

#endif