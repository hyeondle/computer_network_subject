#include "server.h"
#include <netinet/tcp.h>

void operate_order(t_connected *client, int order, char *text);

void *handle_clnt(void *arg) {
    t_connected *client;
    int sock;
    int client_cnt;
    int str_len = 0;
    int order = 0;
    char msg[BUF_SIZE];
    char *text;

    client = (t_connected *)arg;
	sock = client->clnt_sock;
	client_cnt = client->clnt_cnt;

	// connection, name check
	sprintf(msg, "SERVER :: Connect\n");
	write(sock, msg, strlen(msg));
	memset(msg, 0, BUF_SIZE);	//buffer clear (이후 코드에서도 버퍼를 사용 한 후에는 항상 초기화를 했습니다)

	read(sock, client->name, 20); // read name
	while (dup_check(client) == -1) {	// check if name is duplicated
		memset(client->name, 0, 20);
		read(sock, client->name, 20);
	}
	sprintf(msg, "OK\n");
	write(sock, msg, strlen(msg));	// send OK to client
	memset(msg, 0, BUF_SIZE);

	read(sock, msg, BUF_SIZE-1);	// read OK sign from client
	printf("%s", msg);
	memset(msg, 0, BUF_SIZE);

	add_name_list(client);	// add name in map list (for whisper)

	printf("Connected :: client %d's name: %s\n", client_cnt, client->name);

	send_all_j(client);	// send all clients that new client is connected

	// main operation
	while(1) {
		str_len = read(sock, msg, BUF_SIZE);
		msg[strcspn(msg, "\n")] = '\0';
		if (str_len == 0) { // if client disconnected, break
			break;
		}
		printf("Received :: client %d's message: %s\n", client_cnt, msg);
		order = parser(msg, &text);	// parse the message
		operate_order(client, order, text); // 파싱한 내용 기반으로 명령 수행
		if (text) {
			text = NULL;
			free(text);
		}
		if (str_len == 0 || order == EXIT) {
			break;
		}
	}

	// termination
	delete_name_list(client);
	pthread_mutex_lock(client->mutex_list->s_a);
	client_cnt = client->server->clnt_cnt;
	for (int i = 0; i < client_cnt; i++) {
		if (sock == client->server->clnt_socks[i]) {
			while (i++ < client_cnt - 1) {
				client->server->clnt_socks[i] = client->server->clnt_socks[i + 1];
			}
			break;
		}
	}
	pthread_mutex_unlock(client->mutex_list->s_a);

	printf("Disconnected :: client %d's name: %s\n", client->clnt_cnt, client->name);
	close(sock);
	free(client->t_id);
	free(client);

	return 0;
}

void send_not_implemented(t_connected *client) { // 구현이 안된 명령임을 알림
	char msg[BUF_SIZE];

	sprintf(msg, "SERVER :: Not implemented operation\n");
	write(client->clnt_sock, msg, strlen(msg));
}

void operate_order(t_connected *client, int order, char *text) {
	switch (order) {
		case WALL:
			send_all(client, text);
			break;
		case WHISPER:
			whisper(client, text);
			break;
		case IGNORE:
			break;
		// case GAME:
			// game(client, text);
			// break;
		default:
			send_not_implemented(client);
	}
}