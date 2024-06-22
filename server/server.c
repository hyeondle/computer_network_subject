#include "server.h"

t_connected *generate_client(t_server *server, t_mutex_list *mutex_list) {
    t_connected *connected;

    connected = (t_connected *)malloc(sizeof(t_connected));
    if (!connected) {
        perror("malloc");
        exit(1);
    }

    connected->clnt_sock = server->clnt_sock;
    connected->server = server;
    connected->mutex_list = mutex_list;
    connected->t_id = (pthread_t *)malloc(sizeof(pthread_t));
    if (!connected->t_id) {
        perror("malloc");
        exit(1);
    }

    return connected;
}

void run_server(t_server *server, t_mutex_list *mutex_list) {
    t_connected *connected;

    while (1) {
        server->clnt_addr_size = sizeof(server->clnt_addr);
        server->clnt_sock = accept(server->serv_sock, (struct sockaddr *)&server->clnt_addr, (socklen_t *)&server->clnt_addr_size);

        connected = generate_client(server, mutex_list);
        pthread_mutex_lock(mutex_list->s_a);
        connected->clnt_cnt = server->clnt_cnt;
        server->clnt_socks[server->clnt_cnt++] = server->clnt_sock;
        pthread_mutex_unlock(mutex_list->s_a);

        printf("Connected :: client %d's IP: %s\n", connected->clnt_cnt, inet_ntoa(server->clnt_addr.sin_addr));
        pthread_create(connected->t_id, NULL, handle_clnt, (void *)connected);
        pthread_detach(*(connected->t_id));
    }
    close(server->serv_sock);
}
