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
#define Server_IP "127.0.0.1"

int main() {



    int socket_fd; // socket file descriptor
    char buffer[MaxBuffer]; // buffer in which client messages will be stored
    char* message = "Howdy, I am the stupid server!"; // Server message
    struct sockaddr_in server_address, client_address;

    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        printf("Error while creating socket\n");
        return -1;
    }

    // initialize sockaddr_in with 0
    memset(&server_address, 0, sizeof(server_address));
    memset(&client_address, 0, sizeof(client_address));

    char client_address_buffer[INET_ADDRSTRLEN];
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(Server_IP);
    server_address.sin_port = htons(Port);

    // bind socket with server_address
    if (bind(socket_fd, (const struct sockaddr_in*)&server_address, sizeof(server_address)) == -1) {
        printf("Error while binding socket\n");
        inet_ntop(AF_INET, &server_address.sin_addr, client_address_buffer, sizeof(client_address_buffer));
        printf("Address is %s\n", client_address_buffer);
        return -1;
    }


    socklen_t client_size = sizeof(client_address), datagram_size;
    if ((recvfrom(socket_fd, (char*)buffer, MaxBuffer, MSG_WAITALL, (struct sockaddr*)&client_address, &datagram_size) < 0)) {
        printf("Error");
    }
    else {
        printf("Recieved %s\n", buffer);
        memset(&buffer, 0, sizeof(buffer));
    }

    while ((recvfrom(socket_fd, (char*)buffer, MaxBuffer, MSG_WAITALL, (struct sockaddr*)&client_address, &datagram_size)) >= 0) {

        inet_ntop(AF_INET, &client_address.sin_addr, client_address_buffer, sizeof(client_address_buffer));
        printf("\nRecieved From Client with address %s request\n",
            client_address_buffer);
        if (strcmp(buffer, "shutdown") == 0) {
            close(socket_fd);
            printf("Closing server\n");
            return 0;
        }

        printf("Searching for %s\n", buffer);
        //int sourse_fd = open(buffer, O_RDONLY);
        if (access(buffer, F_OK) == 0) {
            printf("File found\n");
            sleep(1);
            char* response = "hit";
            printf("Sending hit signal\n");
            sendto(socket_fd, (const char*)response, strlen(response), MSG_CONFIRM, (struct sockaddr*)&client_address, datagram_size);
        }
        else {
            printf("%s not found, initializing transfer from client\n", buffer);
            char* response = "miss";

            sleep(1);
            printf("Sending miss signal\n");
            //client_address.sin_addr.s_addr = inet_addr(INADDR_ANY);
            //sendto(socket_desc, server_message, strlen(server_message), 0, (struct sockaddr*)&client_addr, client_struct_length)
            //sendto(socket_fd, (const char*)buffer, strlen(buffer), MSG_CONFIRM, (struct sockaddr*)&server_address, datagram_size);

            if (sendto(socket_fd, response, strlen(response), 0, (struct sockaddr*)&client_address, sizeof(client_address)) < 0)
                printf("Cant send message\n");

            printf("Waiting response\n");
            char tempBuffer[MaxBuffer];

            int file_fd = open(buffer, O_WRONLY | O_CREAT | O_EXCL, S_IRWXU & (~S_IXUSR));
            int i = 0;
            while (1) {
                memset(tempBuffer, 0, sizeof(tempBuffer));
                recvfrom(socket_fd, (char*)tempBuffer, MaxBuffer, MSG_WAITALL, (struct sockaddr*)&client_address, &datagram_size);
                inet_ntop(AF_INET, &client_address.sin_addr, client_address_buffer, sizeof(client_address_buffer));
                i++;
                printf("Recieved\n%lu size packet from %s\n", sizeof(tempBuffer), client_address_buffer);

                if (!file_fd) {
                    perror("File error");
                    return -1;
                }

                if (strcmp(tempBuffer, "LoLThatzAll") == 0)
                    break;
                
                write(file_fd, tempBuffer, strlen(tempBuffer));
            }
            printf("Recieved %d packets\n", i);
            printf("Saving the file\n");
            close(file_fd);
            printf("File %s created\n", buffer);
        }

        memset(&buffer, 0, sizeof(buffer));
    }
}