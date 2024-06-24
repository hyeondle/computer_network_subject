#include "server.h"

int find_order(char *order) {
	if (strcmp(order, "EXIT") == 0) {
        return EXIT;
    } else if (strcmp(order, "LOBBY") == 0) {
        return LOBBY;
    } else if (strcmp(order, "HELP") == 0) {
        return HELP;
    } else if (strcmp(order, "WHISPER") == 0) {
        return WHISPER;
    } else if (strcmp(order, "ROOM") == 0) {
        return ROOM;
    } else if (strcmp(order, "CREATE") == 0) {
        return CREATE;
    } else if (strcmp(order, "JOIN") == 0) {
        return JOIN;
    } else if (strcmp(order, "LEAVE") == 0) {
        return LEAVE;
    } else if (strcmp(order, "LIST") == 0) {
        return LIST;
    } else if (strcmp(order, "WALL") == 0) {
        return WALL;
    } else if (strcmp(order, "GAME") == 0) {
        return GAME;
	} else if (strcmp(order, "IGNORE") == 0) {
		return IGNORE;
	} else {
        return -1;
    }
}

int parser(char *msg, char **text) {
	char **tokenized;
	char *temp;
	int return_value;
	int str_len = 0;

	tokenized = ft_split(msg, ':');
	return_value = find_order(tokenized[0]);

	if (return_value == -1) {
		printf("PARSER :: Invalid order\n");
		if (tokenized) {
			for (int i = 0; tokenized[i]; i++) {
				tokenized[i] = NULL;
				free(tokenized[i]);
			}
			tokenized = NULL;
			free(tokenized);
		}
		memset(msg, 0, BUF_SIZE);
		text = NULL;
		return -1;
	}

	if (return_value == GAME) {
		printf("PARSER :: GAME\n");
		*text = strdup(tokenized[1]);
		for (int i = 0; tokenized[i]; i++) {
			tokenized[i] = NULL;
			free(tokenized[i]);
		}
		tokenized = NULL;
		free(tokenized);
		memset(msg, 0, BUF_SIZE);
		return return_value;
	}

	for (int i = 1; tokenized[i]; i++) {
		str_len += strlen(tokenized[i]);
		str_len++;
	}

	temp = (char *)malloc(sizeof(char) * (str_len));
	if (!temp)
		printf("PARSER :: malloc error\n");

	temp[0] = '\0';

	for (int i = 1; tokenized[i]; i++) {
		strcat(temp, tokenized[i]);
		if (tokenized[i + 1]) {
			strcat(temp, ":");
		}
	}

	*text = temp;

	for (int i = 0; tokenized[i]; i++) {
		tokenized[i] = NULL;
		free(tokenized[i]);
	}
	tokenized = NULL;
	free(tokenized);

	memset(msg, 0, BUF_SIZE);
	return return_value;
}

int dup_check(t_connected *client) {
	t_map *list;
	char buf[BUF_SIZE];

	list = client->server->list;
	pthread_mutex_lock(client->mutex_list->map);
	for (int i = 0; i < MAX_CLNT; i++) {
		if  (list[i].key == NULL)
			continue;
		if (strcmp(list[i].key, client->name) == 0) {
			printf("dup occured\n");
			sprintf(buf, "SERVER :: The name is already in use.\n");
			write(client->clnt_sock, buf, strlen(buf));
			memset(buf, 0, BUF_SIZE);
			sprintf(buf, "CONNECT FAIL\n");
			write(client->clnt_sock, buf, strlen(buf));
			memset(buf, 0, BUF_SIZE);
			pthread_mutex_unlock(client->mutex_list->map);
			return -1;
		}
	}
	pthread_mutex_unlock(client->mutex_list->map);
	return 0;
}

int find_name_list(t_map *list, char *name) {
	t_map *temp;

	temp = list;
	for (int i = 0; i < MAX_CLNT; i++) {
		if (temp[i].key != NULL && strcmp(temp[i].key, name) == 0) {
			return temp[i].value;
		}
	}
	return -1;
}

void add_name_list(t_connected *client) {
	t_map *list;

	list = client->server->list;
	pthread_mutex_lock(client->mutex_list->map);
	for (int i = 0; i < MAX_CLNT; i++) {
		if (list[i].key == NULL) {
			printf("ADD_NAME_LIST :: %s, %d\n", client->name, client->clnt_sock);
			list[i].key = strdup(client->name);
			list[i].value = client->clnt_sock;
			pthread_mutex_unlock(client->mutex_list->map);
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
			printf("DELETE_NAME_LIST :: %s, %d\n", client->name, client->clnt_sock);
			free(list[i].key);
			list[i].key = NULL;
			list[i].value = 0;
			break;
		}
	}
	pthread_mutex_unlock(client->mutex_list->map);
}

void send_all_j(t_connected *client) {
	char buf[BUF_SIZE];

	sprintf(buf, "\033[34mLOBBY : %s entered in the server\033[0m\n", client->name);
	pthread_mutex_lock(client->mutex_list->s_a);
	for (int i = 0; i < client->server->clnt_cnt; i++) {
		write(client->server->clnt_socks[i], buf, strlen(buf));
	}
	pthread_mutex_unlock(client->mutex_list->s_a);
}

void send_all(t_connected *client, char *msg) {
	char buf[BUF_SIZE];

	printf("send all");
	sprintf(buf, "%s : %s\n", client->name, msg);
	pthread_mutex_lock(client->mutex_list->s_a);
	for (int i = 0; i < client->server->clnt_cnt; i++) {
		write(client->server->clnt_socks[i], buf, strlen(buf));
	}
	pthread_mutex_unlock(client->mutex_list->s_a);
}

// split
static int str_len_s(char *str, char c)
{
	int i;

	i = 0;
	while (str[i] && str[i] != c) {
		i++;
	}
	return i;
}

static size_t tab_size(char *s, char c)
{
	size_t i;
	size_t k;

	i = 0;
	k = 0;
	if (s[k] == c)
		if (s[k + 1] != c)
			k++;
	while (s[k])
	{
		if (s[k] == c)
		{
			while (s[k + 1] == c)
				k++;
			if (s[k + 1] != '\0' && s[k + 1] != c)
				i++;
		}
		k++;
	}
	return (i + 1);
}

static char *put_word(char *str, char set, size_t c, char **tab)
{
	size_t i;
	size_t size;
	char *word;

	i = 0;
	size = (size_t)str_len_s(str, set);
	word = (char *)malloc(sizeof(char) * (size + 1));
	if (!word)
	{
		while (i < c)
		{
			free(tab[i]);
			tab[i] = NULL;
			i++;
		}
		free(tab);
		return NULL;
	}
	while (i < size)
	{
		word[i] = str[i];
		i++;
	}
	word[i] = '\0';
	return word;
}

static char **do_split(char **tab, char *temp, char c)
{
	size_t i;

	i = 0;
	while (*temp)
	{
		while (*temp && *temp == c)
			temp++;
		if (*temp != 0)
		{
			tab[i] = put_word(temp, c, i, tab);
			if (tab[i] == NULL)
				return (NULL);
			i++;
		}
		while (*temp && !(*temp == c))
			temp++;
	}
	tab[i] = NULL;
	return (tab);
}

char **ft_split(char *s, char c)
{
	size_t i;
	char **tab;
	char *temp;

	if (!s)
		return NULL;
	temp = (char *)s;
	i = 0;
	tab = (char **)malloc(sizeof(char *) * (tab_size(temp, c) + 1));
	if (!tab)
		return NULL;
	tab = do_split(tab, temp, c);
	return (tab);
}