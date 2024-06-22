#include "server.h"

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

	write(sock, "SERVER :: Connect\n", 19);
	read(sock, client->name, 20);

	printf("Connected :: client %d's name: %s\n", client_cnt, client->name);
	send_all(client);
	add_name_list(client);

	memset(msg, 0, BUF_SIZE);

	while(1) {
		// main operation
	}

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
	client->server->clnt_cnt--;
	pthread_mutex_unlock(client->mutex_list->s_a);

	printf("Disconnected :: client %d's name: %s\n", client_cnt, client->name);
	close(sock);
	free(client->t_id);
	free(client);

	return 0;
}