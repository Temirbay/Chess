# include <stdio.h>
# include <string.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <pthread.h>
# include <stdlib.h>

void *connection_handler(void *client_socket);
int numberOfClients = 0;

int client1;
int client2;

int main () {
	int server_socket, read_size;
	struct sockaddr_in server, client;

	if ((server_socket = socket(AF_INET , SOCK_STREAM , 0)) == -1) {
		perror("Socket error");
		return 1;
	}

	printf("Socket created\n");

	server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    if(bind(server_socket, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("Bind failed");
        return 1;
    }

    printf("%s\n", "Binded");

    listen(server_socket , 3);

    read_size = sizeof(struct sockaddr_in);

    while (1) {
        int client_socket = accept(server_socket, (struct sockaddr *)&client, (socklen_t*)&read_size);

        if (client_socket == -1) {
            perror("accept failed");
            return 1;
        }

        numberOfClients++;
        if (numberOfClients == 1)
            client1 = client_socket;

        if (numberOfClients == 2) {
            numberOfClients = 0;
            client2 = client_socket;
        }

        pthread_t thread;
        pthread_create(&thread, NULL, connection_handler, &client_socket);
        printf("Connection accepted\n");
    }
    return 0;
}

void *connection_handler(void *client_socket) {
    int sock = *(int*) client_socket;
    int read_size;
    char client_message[100];

    int a = 0;
    if (numberOfClients == 1) a = 1;

    int cnt = 0;
    while (numberOfClients == 1) {
            if (cnt == 0)
                printf("Wait while your connects...\n");
            cnt = 1;
    }
    int dest_socket = ((sock == client1) ? client2 : client1);

    if (send (sock, &a, sizeof(int), 0) == -1)
        printf("Send failure");

    char start_message[] = "Wait for an opponent!";
    int size = strlen(start_message);
    send(sock, &size, sizeof(size), 0);
    send(sock, start_message, size, 0);

    while ((read_size = recv(sock, &client_message, 100, 0)) != 0) {
        send(dest_socket, &client_message, 100, 0);
    }

    if (read_size == 0) {
        printf("Client #%d: disconnected\n", sock);
        fflush(stdout);
    }

    else if (read_size == -1){
        perror("Receive failure");
    }

    close(sock);
}
