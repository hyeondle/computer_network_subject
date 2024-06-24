#include "server.h"
#include <netinet/tcp.h>
#include <fcntl.h>


void game(t_connected *client, char *text) {
	int usr1, usr2;

	printf("game start\n");

	usr2 = find_name_list(client->server->list, text);
	if (usr2 == -1) {
		write(client->clnt_sock, "NO USER", 7);
		return;
	}

	int addr_size = sizeof(client->server->game_addr);
	write(client->clnt_sock, "GAME START", 10);
	usr1 = accept(client->server->game_sock, (struct sockaddr *)&client->server->game_addr, (socklen_t *)&addr_size);
	write(usr2, "GAME REQUEST", 12);
	usr2 = accept(client->server->game_sock, (struct sockaddr *)&client->server->game_addr, (socklen_t *)&addr_size);

	int flag = 1;
	setsockopt(usr1, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));
	setsockopt(usr2, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));

	printf("connect for game\n");

	int ball_x, ball_y;
	int paddle1, paddle2;

	ball_x = 1280 / 2;
	ball_y = 720 / 2;

	paddle1 = 720 / 2;
	paddle2 = 720 / 2;

	int ball_dir_x = 1;
	int ball_dir_y = 1;

	int p1_win = 0;
	int p2_win = 0;

	char buf[BUF_SIZE];
	char buf2[BUF_SIZE];
	char buf3[BUF_SIZE];
	char buf4[BUF_SIZE];
	char buf5[BUF_SIZE];

	write(usr1, "1", 1);
	write(usr2, "2", 1);

	read(usr1, buf2, BUF_SIZE);
	read(usr2, buf3, BUF_SIZE);
	printf("game start\n");

	int n1 = 0, n2 = 0;

	while (1) {
		usleep(100000);
		memset(buf2, 0, BUF_SIZE);
		memset(buf3, 0, BUF_SIZE);
		sprintf(buf2, "%d %d %d %d", ball_x, ball_y, paddle1, paddle2);
		sprintf(buf3, "%d %d %d %d", ball_x, ball_y, paddle1, paddle2);
		write(usr1, buf2, strlen(buf2));
		write(usr2, buf3, strlen(buf3));

		// if (p1_win || p2_win)
		// 	break;

		memset(buf4, 0, BUF_SIZE);
		memset(buf5, 0, BUF_SIZE);
		n1 = read(usr1, buf4, BUF_SIZE);
		n2 = read(usr2, buf5, BUF_SIZE);

		if (n1 < 0 || n2 < 0) continue;

		if (n1 > 0) paddle1 = atoi(buf2);
		if (n2 > 0) paddle2 = atoi(buf3);

		ball_x += ball_dir_x;
		ball_y += ball_dir_y;

		if (ball_y <= 0 || ball_y >= 720) {
			ball_dir_y *= -1;
		}

		// if (ball_x <= 0) {
		// 	p2_win++;
		// }

		// if (ball_x >= 1280) {
		// 	p1_win++;
		// }

		if (ball_x == 20 && ball_y >= paddle1 && ball_y <= paddle1 + 100) {
			ball_dir_x *= -1;
		}

		if (ball_x == 1260 && ball_y >= paddle2 && ball_y <= paddle2 + 100) {
			ball_dir_x *= -1;
		}

		usleep(16000);
	}

}