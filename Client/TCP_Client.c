#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MaxBuffer 1500
#define Port 8080

#define Msg "Hey stupid server can u hear me?"
#define Server_IP "127.0.0.1"
#define Host_IP "127.0.0.1"

int main() {
    //printf("Hey");
    int socket_fd,connect_fd; // socket file descriptor
    char buffer[MaxBuffer]; // buffer in which client messages will be stored
    char filename[100];
    struct sockaddr_in server_address, income_address, client_address;

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("Error while creating socket\n");
        return -1;
    }

    // initialize sockaddr_in with 0
    memset(&server_address, 0, sizeof(server_address));
    memset(&income_address, 0, sizeof(income_address));

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(Server_IP);
    server_address.sin_port = htons(Port);

    // the current client
    client_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(Host_IP);
    client_address.sin_port = htons(Port);


    socklen_t server_size = sizeof(server_address);

    connect_fd = connect(socket_fd, (struct sockaddr*)&server_address, server_size);
    while (connect_fd != 0) {
        printf("Server accept failed... press any key to try again\n");
        getchar();
        connect_fd = connect(socket_fd, (struct sockaddr*)&server_address, server_size);
    
    }
    printf("Server accepted the client...\n");

    
    while (printf("Enter file name > ")) {
        scanf("%s", buffer);
        memcpy(filename, buffer, strlen(buffer) + 1);
        
        int file_fd = open(filename, O_RDONLY);
        if (file_fd == -1) {
            printf("There is no such file in your directory\n");
            memset(&buffer, 0, sizeof(buffer));
            continue;
        }
        printf("Sending \"%s\" to Server\n", buffer);
        write(socket_fd, buffer, sizeof(buffer));
        if (strcmp(buffer, "shutdown") == 0) {
            close(connect_fd);
            close(socket_fd);
            printf("Closing client\n");
            return 0;
        }

        // wait to see if the file exist on server
        memset(&buffer, 0, sizeof(buffer));
        printf("Waiting response from server\n");
        read(socket_fd, (char*)buffer, sizeof(buffer));

        // response actions
        printf("Recieved %s from server\n", buffer);
        if (strcmp(buffer, "miss") == 0) {
            printf("Caught response, file miss\n");
            memset(&buffer, 0, sizeof(buffer));
            printf("Preparing to send %s file to server\n", filename);
            if (file_fd == -1) {
                printf("There is no such file in your directory\n");
                memset(&buffer, 0, sizeof(buffer));
                continue;
            }

            int i = 1;
            while (read(file_fd, &buffer, MaxBuffer) != 0) {
                printf("Sending %d segment\n", i);
                write(socket_fd, buffer, sizeof(buffer));
                memset(&buffer, 0, sizeof(buffer));
                //usleep(10);
                i++;
            }
            //printf("Sending %d segment\n", i);
            //write(socket_fd, buffer, sizeof(buffer));
            write(socket_fd, "LoLThatzAll", sizeof("LoLThatzAll"));
            close(file_fd);
            
        }
        else if (strcmp(buffer, "hit") == 0) {
            printf("Caught response, file exist\n");
        }
        memset(&buffer, 0, sizeof(buffer));
    }
}

/*

    First check if file exist then send request
*/