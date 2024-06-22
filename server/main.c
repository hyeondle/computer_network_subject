#include "server.h"

int main(int argc, char **argv) {
    t_server *server;
    t_mutex_list *mutex_list;

    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    mutex_list = init_mutex();
    server = init_server(argv);

    run_server(server, mutex_list);
}