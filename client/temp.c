#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUF_SIZE 100
#define NAME_SIZE 20

void * send_msg(void * arg);
void * recv_msg(void * arg);
void error_handling(char * msg);

char name[NAME_SIZE]="[DEFAULT]";
char msg[BUF_SIZE];

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    pthread_t snd_thread, rcv_thread;
    void * thread_return;
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

    memset(msg, 0, BUF_SIZE);

    read(sock, msg, BUF_SIZE-1);
    msg[strcspn(msg, "\n")] = '\0';
    printf("%s\n", msg);
    memset(msg, 0, BUF_SIZE);
    printf("\n");
    write(sock, name, strlen(name));
    read(sock, msg, BUF_SIZE-1);
    msg[strcspn(msg, "\n")] = '\0';
    printf("%s\n", msg);
    memset(msg, 0, BUF_SIZE);
    printf("\n");
    read(sock, msg, BUF_SIZE-1);
    msg[strcspn(msg, "\n")] = '\0';
    printf("n:%s\n", msg);
    printf("%d\n", strcmp(msg, "0"));
    printf("\n");
    // memset(msg, 0, BUF_SIZE);

    while (strcmp(msg, "0\n") != 0) {
        memset(msg, 0, BUF_SIZE);
        write(1, "Name already exists. Please enter a new name\n", 45);
        read(STDIN_FILENO, name, NAME_SIZE);
        name[strcspn(name, "\n")] = '\0';
        write(sock, name, strlen(name));
        read(sock, msg, BUF_SIZE-1);
        printf("%s\n", msg);
        read(sock, msg, BUF_SIZE-1);
        memset(msg, 0, BUF_SIZE);
    }
    memset(msg, 0, BUF_SIZE);
    write(sock, name, strlen(name));
    printf("Connected to server\n");
    pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);
    pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);
    pthread_join(snd_thread, &thread_return);
    pthread_join(rcv_thread, &thread_return);
    close(sock);
    return 0;
}

void * send_msg(void * arg)   // send thread main
{
    int sock=*((int*)arg);
    char name_msg[BUF_SIZE];
    while(1)
    {
        fgets(msg, BUF_SIZE, stdin);
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