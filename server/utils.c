#include "server.h"

void add_name_list(t_connected *client) {
	t_map *list;

	list = client->server->list;
	pthread_mutex_lock(client->mutex_list->map);
	for (int i = 0; i < MAX_CLNT; i++) {
		if (list[i].key == NULL) {
			list[i].key = strdup(client->name);
			list[i].value = client->clnt_sock;
			break;
		}
	}
	pthread_mutex_unlock(client->mutex_list->map);
}

void delete_name_list(t_connected *client) {
	t_map *list;

	list = client->server->list;
	pthread_mutex_lock(client->mutex_list->map);
	for (int i = 0; i < MAX_CLNT; i++) {
		if (list[i].key == NULL) {
			continue;
		}
		if (strcmp(list[i].key, client->name) == 0) {
			free(list[i].key);
			list[i].key = NULL;
			list[i].value = 0;
			break;
		}
	}
	pthread_mutex_unlock(client->mutex_list->map);
}

void send_all(t_connected *client) {
	char buf[BUF_SIZE];

	sprintf(buf, "LOBBY : %s entered in the server\n", client->name);
	pthread_mutex_lock(client->mutex_list->s_a);
	for (int i = 0; i < client->server->clnt_cnt; i++) {
		write(client->server->clnt_socks[i], buf, strlen(buf));
	}
	pthread_mutex_unlock(client->mutex_list->s_a);
}