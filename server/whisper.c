#include "server.h"

void whisper(t_connected *client, char *text) {
	t_map *list;
	int client_cnt = 0;
	char *name;
	char msg[BUF_SIZE];
	char **tokenized;
	char **target;
	int *target_sock;
	char *temp;
	int str_len = 0;

	list = client->server->list;

	msg[0] = '\0';

	tokenized = ft_split(text, ' ');
	if (!tokenized) {
		printf("WHISPER :: CANNOT SPLIT MESSAGE\n");
		return;
	}

	for (int i = 0; tokenized[0][i]; i++) {
		if (tokenized[0][i] == '@')
			client_cnt++;
	}

	target_sock = (int *)malloc(sizeof(int) * (client_cnt));
	if (!target_sock) {
		printf("WHISPER :: malloc error\n");
		return;
	}

	target = ft_split(tokenized[0], '@');
	if (!target) {
		printf("WHISPER :: CANNOT SPLIT MESSAGE\n");
		return;
	}

	pthread_mutex_lock(client->mutex_list->map);
	for (int i = 0; target[i]; i++) {
		for (int j = 0; j < MAX_CLNT; j++) {
			if (list[j].key != NULL && strcmp(list[j].key, target[i]) == 0) {
				printf("WHISPER :: TARGET %s FOUND\n", target[i]);
				target_sock[i] = list[j].value;
			}
		}
	}
	pthread_mutex_unlock(client->mutex_list->map);

	if (!target_sock[0]) {
		printf("WHISPER :: NO TARGET\n");
		sprintf(msg, "SERVER :: No target to whisper\n");
		write(client->clnt_sock, msg, strlen(msg));
		return;
	}

	for (int i = 1; tokenized[i]; i++) {
		str_len += strlen(tokenized[i]);
		str_len++;
	}

	temp = (char *)malloc(sizeof(char) * (str_len));
	if (!temp)
		printf("WHISPER :: malloc error\n");

	temp[0] = '\0';

	for (int i = 1; tokenized[i]; i++) {
		strcat(temp, tokenized[i]);
		if (tokenized[i + 1]) {
			strcat(temp, " ");
		}
	}
	temp[str_len] = '\0';

	sprintf(msg, "%s : %s\n", client->name, temp);
	for (int i = 0; target_sock[i]; i++) {
		write(target_sock[i], msg, strlen(msg));
	}

	temp = NULL;
	free(temp);
	for (int i = 0; tokenized[i]; i++) {
		tokenized[i] = NULL;
		free(tokenized[i]);
	}
	tokenized = NULL;
	free(tokenized);
	for (int i = 0; target[i]; i++) {
		target[i] = NULL;
		free(target[i]);
	}
	target = NULL;
	free(target);

	memset(msg, 0, BUF_SIZE);
	return;
}