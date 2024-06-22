#include "server.h"

t_mutex_list *init_mutex() {
	t_mutex_list *mutex_list;

	mutex_list = (t_mutex_list *)malloc(sizeof(t_mutex_list));
	if (!mutex_list) {
		perror("malloc");
		exit(1);
	}

	mutex_list->s_a = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	if (!mutex_list->s_a) {
		perror("malloc");
		exit(1);
	}
	pthread_mutex_init(mutex_list->s_a, NULL);

	return mutex_list;
}

t_server *init_server(char **argv) {
	t_server *server;

	server = (t_server *)malloc(sizeof(t_server));
	if (!server) {
		perror("malloc");
		exit(1);
	}

	server->serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (server->serv_sock == -1) {
		perror("socket");
		exit(1);
	}

	memset(&server->serv_addr, 0, sizeof(server->serv_addr));
	server->serv_addr.sin_family = AF_INET;
	server->serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server->serv_addr.sin_port = htons(atoi(argv[1]));

	if (bind(server->serv_sock, (struct sockaddr *)&server->serv_addr, sizeof(server->serv_addr)) == -1) {
		perror("bind");
		exit(1);
	}

	if (listen(server->serv_sock, 5) == -1) {
		perror("listen");
		exit(1);
	}

	server->clnt_cnt = 0;
	server->clnt_addr_size = sizeof(server->clnt_addr);

	return server;
}
