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

	int flag = 1;
	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void *)&flag, sizeof(int));


	// basic condition check
	/*
	write(sock, "SERVER :: Connect\n", 19);
	read(sock, client->name, 20);
	while (dup_check(client) == -1) {
		memset(client->name, 0, 20);
		read(sock, client->name, 20);
	}
	sprintf(msg, "SERVER :: Welcome, %s\n", client->name);
	write(sock, msg, strlen(msg));
	printf("welcome msg\n");
	memset(msg, 0, BUF_SIZE);
	sprintf(msg, "CONNECT OK\n");
	// write(sock, msg, strlen(msg));
	read(sock, msg, BUF_SIZE-1);
	memset(msg, 0, BUF_SIZE);
	write(sock, "CONNECT OK\n", 11);
	printf("send 0 seq\n");
	read(sock, msg, BUF_SIZE-1);
	printf("read seq\n");
	add_name_list(client);
	printf("add seq done\n");
	printf("Connected :: client %d's name: %s\n", client_cnt, client->name);
	// send_all_j(client);
	*/
	sprintf(msg, "SERVER :: Connect\n");
	write(sock, msg, strlen(msg));
	memset(msg, 0, BUF_SIZE);
	read(sock, client->name, 20);
	while (dup_check(client) == -1) {
		memset(client->name, 0, 20);
		read(sock, client->name, 20);
	}
	sprintf(msg, "OK\n");
	write(sock, msg, strlen(msg));
	memset(msg, 0, BUF_SIZE);
	read(sock, msg, BUF_SIZE-1);
	printf("%s", msg);

	add_name_list(client);

	printf("Connected :: client %d's name: %s\n", client_cnt, client->name);

	memset(msg, 0, BUF_SIZE);

	pthread_mutex_lock(client->mutex_list->map);
	t_map *list = client->server->list;
	for (int i = 0; i < MAX_CLNT; i++) {
		printf("list[%d] : %s\n", i, list[i].key);
	}
	pthread_mutex_unlock(client->mutex_list->map);

	// main operation
	while(1) {
		str_len = read(sock, msg, BUF_SIZE);
		if (str_len == 0) {
			break;
		}
		printf("Received :: client %d's message: %s\n", client_cnt, msg);
		order = parser(msg, &text);
		operate_order(client, order, text);
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

void operate_order(t_connected *client, int order, char *text) {
	switch (order) {
		case LOBBY:
			send_all(client, text);
			break;
		case EXIT:
			break;
		case WHISPER:
			whisper(client, text);
			break;
		default:
			break;
	}
}