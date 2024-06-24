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

typedef struct s_mlx {
	void *mlx;
	void *win;
	void *img;
	char *addr;
	int bpp;
	int size_line;
	int endian;
} t_mlx;

typedef struct s_sockinfo {
	int sock;
	pthread_mutex_t *game_mutex;
	int game_started;
	struct sockaddr_in serv_addr;
	int game_sock;
	struct sockaddr_in game_addr;
	t_mlx *mlx;

	int *loop;
} t_sockinfo;

char **ft_split(char *s, char c);
void *game(void *arg);

#endif