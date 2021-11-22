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
    int socket_fd; // socket file descriptor
    char buffer[MaxBuffer]; // buffer in which client messages will be stored
    char filename[100];
    struct sockaddr_in server_address, income_address, client_address;

    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
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


    int server_size = sizeof(server_address);
    socklen_t datagram_size = sizeof(server_address);
    socklen_t server_struct_size;

    if (sendto(socket_fd, Msg, strlen(Msg), MSG_CONFIRM, (struct sockaddr*)&server_address, datagram_size) < 0) {
        printf("Failed to send first message\n");
    }
    else {
        printf("First message sent\n");
    }

    
    while (printf("Enter file name > ")) {
        scanf("%s", buffer);
        memcpy(filename, buffer, strlen(buffer) + 1);
        printf("Sending \"%s\" to Server\n", buffer);
        
        sendto(socket_fd, (const char*)buffer, strlen(buffer), MSG_CONFIRM, (struct sockaddr*)&server_address, datagram_size);
        if (strcmp(buffer, "shutdown") == 0) {
            close(socket_fd);
            printf("Closing client\n");
            return 0;
        }
        
        /*
        if (bind(socket_fd, (const struct sockaddr_in*)&client_address, sizeof(client_address)) == -1) {
            printf("Error while binding socket\n");
            return -1;
        }
        */

        // wait to see if the file exist on server
        memset(&buffer, 0, sizeof(buffer));
        printf("Waiting response from server\n");
        //if(recvfrom(socket_desc, server_message, sizeof(server_message), 0, (struct sockaddr*)&server_addr, &server_struct_length))
        recvfrom(socket_fd, (char*)buffer, MaxBuffer, 0, (struct sockaddr*)&server_address, &server_struct_size);


        // response actions
        printf("Recieved %s from server\n", buffer);
        if (strcmp(buffer, "miss") == 0) {
            printf("Caught response, file miss\n");
            memset(&buffer, 0, sizeof(buffer));
            int file_fd = open(filename, O_RDONLY);
            printf("Preparing to send %s file to server\n", filename);
            if (file_fd == -1) {
                printf("There is no such file in your directory\n");
                memset(&buffer, 0, sizeof(buffer));
                continue;
            }

            int i = 1;
            while (read(file_fd, &buffer, MaxBuffer) != 0) {
                printf("Sending %d segment\n", i);
                sendto(socket_fd, (const char*)buffer, strlen(buffer), MSG_CONFIRM, (struct sockaddr*)&server_address, datagram_size);
                //usleep(10);
                i++;
            }
            printf("Sending %d segment\n", i+1);
            sendto(socket_fd, (const char*)buffer, strlen(buffer), MSG_CONFIRM, (struct sockaddr*)&server_address, datagram_size);
            sendto(socket_fd, "LoLThatzAll", strlen("LoLThatzAll"), MSG_CONFIRM, (struct sockaddr*)&server_address, datagram_size);
            close(file_fd);
            
        }
        else if (strcmp(buffer, "hit") == 0) {
            printf("Caught response, file exist\n");
        }

        
        memset(&buffer, 0, sizeof(buffer));
    }
}