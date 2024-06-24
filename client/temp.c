#include "client.h"

void * send_msg(void * arg);
void * recv_msg(void * arg);
void error_handling(char * msg);

char name[NAME_SIZE]="[DEFAULT]";
// char msg[BUF_SIZE];

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    pthread_t snd_thread, rcv_thread;
    void * thread_return;

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

    int flag = 1;
	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void *)&flag, sizeof(int));

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
    pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);
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

char *parse_input(char *msg) {
    char *text;
    char **tokenized;
    int order = 0;

    tokenized = ft_split(msg, ' ');

    if (!tokenized) {
        printf("PARSER :: split error\n");
        return NULL;
    }

    if (strcmp(tokenized[0][0], '/') != 0) {
        for (int i = 0; tokenized[i]; i++) {
            tokenized[i] = NULL;
            free(tokenized[i]);
        }
        tokenized = NULL;
        free(tokenized);
        return msg;
        // i think i should use strdup to return the string
    }

    order = find_order(tokenized[0]);

    if (order == -1) {
        printf("PARSER :: Invalid order\n");
        for (int i = 0; tokenized[i]; i++) {
            tokenized[i] = NULL;
            free(tokenized[i]);
        }
        tokenized = NULL;
        free(tokenized);
        return msg;
    }

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
        sprintf(name_msg,"%s", msg);
        write(sock, name_msg, strlen(name_msg));
    }
    return NULL;
}

void * recv_msg(void * arg)   // read thread main
{

    int sock=*((int*)arg);
    char name_msg[NAME_SIZE+BUF_SIZE];
    int str_len;
    while(1)
    {
        str_len=read(sock, name_msg, NAME_SIZE+BUF_SIZE-1);
        if(str_len==-1)
            return (void*)-1;
        name_msg[str_len]=0;
        write(1, name_msg, strlen(name_msg));
    }
    return NULL;
}

void error_handling(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}