#include "./mlx/mlx.h"
#include "client.h"
#include <fcntl.h>
#include <string.h>
#include <unistd.h>


void *game(void *arg) {
    t_sockinfo *sockinfo = (t_sockinfo *)arg;
	t_mlx *mlx = sockinfo->mlx;
    struct sockaddr_in game_addr = sockinfo->game_addr;


    if (connect(sockinfo->game_sock, (struct sockaddr*)&game_addr, sizeof(game_addr)) == -1) {
        perror("connect");
    }

	pthread_mutex_lock(sockinfo->game_mutex);
	sockinfo->game_started = 1;
	pthread_mutex_unlock(sockinfo->game_mutex);
	usleep(10000);

	pthread_mutex_lock(sockinfo->game_mutex);
	while (sockinfo->game_started) {
		pthread_mutex_unlock(sockinfo->game_mutex);
		usleep(10000);
		pthread_mutex_lock(sockinfo->game_mutex);
	}
	pthread_mutex_unlock(sockinfo->game_mutex);

	char *pixel_addr;
	int color;

	int white = 0xFFFFFF;
	int black = 0x000000;
	int red = 0xFF0000;
	int blue = 0x0000FF;

	char buf[1024];
	char buf2[1024];

	int ball_x = 0, ball_y = 0;
	int paddle1 = 0, paddle2 = 0;
	int p1_win = 0, p2_win = 0;

	int me = 0;
	read(sockinfo->game_sock, buf, 1024);
	if (buf[0] == '1')
		me = 1;
	else
		me = 2;
	memset(buf, 0, 1024);

	printf("I'm %d\n", me);

	write(sockinfo->game_sock, "OK", 1);

	int sock = sockinfo->game_sock;

	int n = 0;
	while (1) {
		usleep(100000);
		memset(buf, 0, 1024);
		n = read(sock, buf, 1024);
		if (n < 0) continue;
		if (n > 0) sscanf(buf, "%d %d %d %d", &ball_x, &ball_y, &paddle1, &paddle2);

		// if (p1_win || p2_win) {
		// 	printf("%d %d\n", p1_win, p2_win);
		// 	printf("done\n");
		// 	break;
		// }

		memset(buf2, 0, 1024);
		sprintf(buf2, "%d", paddle1);
		write(sock, buf2, strlen(buf2));

		for (int i = 0; i < 1280; i++) {
			for (int j = 0; j < 720; j++) {
				pixel_addr = mlx->addr + (i * (mlx->bpp / 8) + j * mlx->size_line);
				if (i >= ball_x - 5 && i <= ball_x + 5 && j >= ball_y - 5 && j <= ball_y + 5)
					color = white;
				else if (i >= 40 && i <= 60 && j >= paddle1 - 50 && j <= paddle1 + 50)
					color = red;
				else if (i >= 1220 && i <= 1240 && j >= paddle2 - 50 && j <= paddle2 + 50)
					color = blue;
				else
					color = black;
				*(unsigned int *)pixel_addr = color;
			}
		}
		mlx_put_image_to_window(mlx->mlx, mlx->win, mlx->img, 0, 0);

		
	}

}
