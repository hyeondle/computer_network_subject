#include "client.h"
#include "./mlx/mlx.h"
#include <pthread.h>

void * send_msg(void * arg);
void * recv_msg(void * arg);
void error_handling(char * msg);

char name[NAME_SIZE]="[DEFAULT]";
// char msg[BUF_SIZE];



int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    struct sockaddr_in game_addr;
    pthread_t snd_thread, rcv_thread;
    void * thread_return;
    t_sockinfo *sockinfo;

    char msg[BUF_SIZE];
    char chk[BUF_SIZE];
    char name[NAME_SIZE];

    if(argc!=4) {
        printf("Usage : %s <IP> <port> <name>\n", argv[0]);
        exit(1);
    }

    sprintf(name, "%s", argv[3]);
    sock=socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_addr.sin_port=htons(atoi(argv[2]));

    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling("connect() error");

    sockinfo = (t_sockinfo *)malloc(sizeof(t_sockinfo));
    if (!sockinfo) {
        perror("malloc");
        exit(1);
    }

    sockinfo->loop = (int *)malloc(sizeof(int));
    *(sockinfo->loop) = 0;

    sockinfo->sock = sock;
    sockinfo->serv_addr = serv_addr;
    sockinfo->game_started = 0;
    sockinfo->game_sock = socket(PF_INET, SOCK_STREAM, 0);
    sockinfo->game_mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    if (!sockinfo->game_mutex) {
        perror("malloc");
        exit(1);
    }
    pthread_mutex_init(sockinfo->game_mutex, NULL);

    memset(&game_addr, 0, sizeof(game_addr));
    game_addr.sin_family = AF_INET;
    game_addr.sin_addr.s_addr = inet_addr(argv[1]);
    game_addr.sin_port = htons(atoi(argv[2]) + 1);

    sockinfo->game_addr = game_addr;

    sockinfo->mlx = (t_mlx *)malloc(sizeof(t_mlx));
    if (!sockinfo->mlx) {
        perror("malloc");
        exit(1);
    }

    int flag = 1;
	setsockopt(sockinfo->game_sock, IPPROTO_TCP, TCP_NODELAY, (void *)&flag, sizeof(int));

    memset(msg, 0, BUF_SIZE);

    read(sock, msg, BUF_SIZE-1);
    printf("%s", msg);
    write(sock, name, strlen(name));
    read(sock, msg, BUF_SIZE-1);
    msg[strcspn(msg, "\n")] = '\0';
    while (strcmp(msg, "OK") != 0) {
        memset(msg, 0, BUF_SIZE);
        write(1, "Name already exists. Please enter a new name\n", 45);
        read(STDIN_FILENO, name, NAME_SIZE);
        name[strcspn(name, "\n")] = '\0';
        write(sock, name, strlen(name));
        read(sock, msg, BUF_SIZE-1);
        msg[strcspn(msg, "\n")] = '\0';
    }
    write(sock, "CONNECT OK\n", 11);

    printf("Connected to server\n");
    pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);
    pthread_create(&rcv_thread, NULL, recv_msg, (void*)sockinfo);

    pthread_mutex_lock(sockinfo->game_mutex);
    while (!sockinfo->game_started) {
        pthread_mutex_unlock(sockinfo->game_mutex);
        usleep(10000); // 10ms 대기
        pthread_mutex_lock(sockinfo->game_mutex);
    }
    pthread_mutex_unlock(sockinfo->game_mutex);

    sockinfo->mlx->mlx = mlx_init();
    sockinfo->mlx->win = mlx_new_window(sockinfo->mlx->mlx, 1280, 720, "Pong");
    sockinfo->mlx->img = mlx_new_image(sockinfo->mlx->mlx, 1280, 720);
    sockinfo->mlx->addr = mlx_get_data_addr(sockinfo->mlx->img, &sockinfo->mlx->bpp, &sockinfo->mlx->size_line, &sockinfo->mlx->endian);

    pthread_mutex_lock(sockinfo->game_mutex);
    sockinfo->game_started = 0;
    pthread_mutex_unlock(sockinfo->game_mutex);


    // mlx_key_hook(game.win, key_press, &game);
    // mlx_loop_hook(game.mlx, render_next_frame, &game);
    // mlx_loop(sockinfo->mlx->mlx);

    pthread_join(snd_thread, &thread_return);
    pthread_join(rcv_thread, &thread_return);
    close(sock);
    return 0;
}

int find_order(char *order) {
    if (strcmp(order, "/?") == 0) {
        return HELP;
    } else if (strcmp(order, "/w") == 0) {
        return WHISPER;
    } else if (strcmp(order, "/l") == 0) {
        return LOBBY;
    } else if (strcmp(order, "/r") == 0) {
        return ROOM;
    } else if (strcmp(order, "/a") == 0) {
        return WALL;
    } else if (strcmp(order, "/g") == 0) {
        return GAME;
    } else {
        return -1;
    }
}

char *make_order(char **tokenized, int order) {
    char *text;
    int str_len = 0;

    for (int i = 1; tokenized[i]; i++) {
        str_len += strlen(tokenized[i]);
        str_len++;
    }

    if (order == WHISPER) {
        text = (char *)malloc(sizeof(char) * (str_len + 1 + strlen("WHISPER:")));
        sprintf(text, "WHISPER:");
        for (int i = 1; tokenized[i]; i++) {
            strcat(text, tokenized[i]);
            strcat(text, " ");
        }
    } else if (order == LOBBY) {
        text = (char *)malloc(sizeof(char) * (str_len + 1 + strlen("LOBBY:")));
        sprintf(text, "LOBBY:");
        for (int i = 1; tokenized[i]; i++) {
            strcat(text, tokenized[i]);
            strcat(text, " ");
        }
    } else if (order == ROOM) {
        text = (char *)malloc(sizeof(char) * (str_len + 1 + strlen("ROOM:")));
        sprintf(text, "ROOM:");
        for (int i = 1; tokenized[i]; i++) {
            strcat(text, tokenized[i]);
            strcat(text, " ");
        }
    } else if (order == WALL) {
        text = (char *)malloc(sizeof(char) * (str_len + 1 + strlen("WALL:")));
        sprintf(text, "WALL:");
        for (int i = 1; tokenized[i]; i++) {
            strcat(text, tokenized[i]);
            strcat(text, " ");
        }
    } else if (order == GAME) {
        text = (char *)malloc(sizeof(char) * (str_len + 1 + strlen("GAME:")));
        sprintf(text, "GAME:");
        for (int i = 1; tokenized[i]; i++) {
            strcat(text, tokenized[i]);
            strcat(text, " ");
        }
    } else if (order == IGNORE) {
        text = (char *)malloc(sizeof(char) * (str_len + 1 + strlen("IGNORE:")));
        sprintf(text, "IGNORE:");
        for (int i = 1; tokenized[i]; i++) {
            strcat(text, tokenized[i]);
            strcat(text, " ");
        }
    } else {
        text = (char *)malloc(sizeof(char) * (strlen("INVALID ORDER") + 1));
        sprintf(text, "INVALID ORDER");
    }

    return text;
}

char *parse_input(char *msg) {
    char *text;
    char **tokenized;
    int order = 0;

    tokenized = ft_split(msg, ' ');

    //check if just type abcd
    if (!tokenized) {
        printf("PARSER :: split error\n");
        return NULL;
    }

    if (tokenized[0][0] != '/') {
        for (int i = 0; tokenized[i]; i++) {
            tokenized[i] = NULL;
            free(tokenized[i]);
        }
        tokenized = NULL;
        free(tokenized);
        text = (char *)malloc(sizeof(char) * (strlen(msg) + 1 + strlen("WALL:")));
        sprintf(text, "WALL:%s", msg);
        return text;
    }

    order = find_order(tokenized[0]);

    if (order == -1) {
        // printf("PARSER :: Invalid order\n");
        for (int i = 0; tokenized[i]; i++) {
            tokenized[i] = NULL;
            free(tokenized[i]);
        }
        tokenized = NULL;
        free(tokenized);
        return strdup(msg);
    }
    if (!tokenized[1]) {
        printf("PARSER :: No text\n");
        for (int i = 0; tokenized[i]; i++) {
            tokenized[i] = NULL;
            free(tokenized[i]);
        }
        tokenized = NULL;
        free(tokenized);
        return strdup(msg);
    }

    text = make_order(tokenized, order);

    return text;
}


void * send_msg(void * arg)   // send thread main
{
    char msg[BUF_SIZE];
    int sock=*((int*)arg);
    char name_msg[BUF_SIZE];
    char *text;

    while(1)
    {
        fgets(msg, BUF_SIZE, stdin);
        text = parse_input(msg);
        if(!strcmp(msg,"q\n")||!strcmp(msg,"Q\n"))
        {
            close(sock);
            exit(0);
        }
        if (text == NULL)
            continue;
        write(sock, text, strlen(text));
        memset(msg, 0, BUF_SIZE);
        memset(text, 0, strlen(text));
        text = NULL;
        free(text);
    }
    return NULL;
}

void * recv_msg(void * arg)   // read thread main
{
    pthread_t game_thread;
    t_sockinfo *sockinfo = (t_sockinfo *)arg;
    int sock = sockinfo->sock;
    char name_msg[NAME_SIZE+BUF_SIZE];
    int str_len;
    while(1)
    {
        str_len=read(sock, name_msg, NAME_SIZE+BUF_SIZE-1);
        if(str_len==-1)
            return (void*)-1;
        name_msg[str_len]=0;
        name_msg[strcspn(name_msg, "\n")] = '\0';
        write(1, name_msg, strlen(name_msg));
        write(1, "\n", 1);
        if (strcmp(name_msg, "GAME START") == 0) {
            pthread_create(&game_thread, NULL, game, (void*)sockinfo);
            pthread_detach(game_thread);
        } else if (strcmp(name_msg, "GAME REQUEST") == 0) {
            printf("GAME REQUEST\n");
            pthread_create(&game_thread, NULL, game, (void*)sockinfo);
            pthread_detach(game_thread);
        } else if (strcmp(name_msg, "GAME END") == 0) {
            pthread_mutex_lock(sockinfo->game_mutex);
            sockinfo->game_started = -1;
            pthread_mutex_unlock(sockinfo->game_mutex);
        }
    }
    return NULL;
}

void error_handling(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}