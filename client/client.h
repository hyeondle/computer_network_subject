#ifndef CLIENT_H
# define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/tcp.h>


#define BUF_SIZE 100
#define NAME_SIZE 20

typedef enum e_order {
	HELP,
	WHISPER,
	LOBBY,
	ROOM,
	WALL,
	GAME,
	IGNORE
} t_order;

char **ft_split(char *s, char c);


#endif