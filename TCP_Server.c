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
#define Host_IP "127.0.0.1"

int main() {
    int socket_fd, connect_fd; // socket file descriptor
    char buffer[MaxBuffer]; // buffer in which client messages will be stored
    char* message = "Howdy, I am the stupid server!"; // Server message
    struct sockaddr_in server_address, client_address;

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
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
    if (bind(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        printf("Error while binding socket\n");
        inet_ntop(AF_INET, &server_address.sin_addr, client_address_buffer, sizeof(client_address_buffer));
        printf("Address is %s\n", client_address_buffer);
        close(socket_fd);
        close(connect_fd);
        return -1;
    }

    if ((listen(socket_fd, 1)) != 0) {
        printf("Listen failed...\n");
        return -1;
    }
    else
        printf("Waitng for connection..\n");


    // Accept the data packet from client and verification
    socklen_t client_length = sizeof(client_address);
    connect_fd = accept(socket_fd, (struct sockaddr*)&client_address, &client_length);
    if (connect_fd < 0) {
        printf("Server accept failed...\n");
        return -1;
    }
    else
        printf("Server accepted the client...\n");

    while (1) {

        memset(&buffer, 0, sizeof(buffer));
        if (read(connect_fd, buffer, sizeof(buffer)) < 0) {
            printf("Error occured while reading from client, exiting...\n");
            return -1;
        }
        if (strcmp(buffer, "shutdown") == 0) {
            printf("Caught exit signal\n");
            close(connect_fd);
            close(socket_fd);
            return 0;
        }
        else {
            printf("Searching for %s\n", buffer);
            //int sourse_fd = open(buffer, O_RDONLY);
            if (access(buffer, F_OK) == 0) {
                printf("File found\n");
                sleep(1);
                
                printf("Sending hit signal\n");
                while (write(connect_fd, "hit", sizeof("hit")) < 0) { // Send hit
                    printf("Something went wrong while sending hit response, press any key to try again\n");
                    getchar();
                }
            }

            else {

                printf("%s not found, initializing transfer from client\n", buffer);

                sleep(1);
                printf("Sending miss signal\n");

                while (write(connect_fd, "miss", sizeof("miss")) < 0) { // Send miss
                    printf("Something went wrong while sending miss response, press any key to try again\n");
                    getchar();
                }

                printf("Waiting response\n");
                char tempBuffer[MaxBuffer];
                int file_fd = open(buffer, O_WRONLY | O_CREAT | O_EXCL, S_IRWXU & (~S_IXUSR));
                int i = 1;
                while (1) {
                    memset(tempBuffer, 0, sizeof(tempBuffer));
                    if (read(connect_fd, tempBuffer, sizeof(tempBuffer)) < 0) {
                        printf("Error occured while reading from client, exiting...\n");
                        return -1;
                    }
                    
                    printf("Recieved %d packet with size %lu\n", i, sizeof(tempBuffer));
                    i++;

                    if (!file_fd) {
                        perror("File error");
                        return -1;
                    }

                    if (strcmp(tempBuffer, "LoLThatzAll") == 0)
                        break;

                    write(file_fd, tempBuffer, strlen(tempBuffer) - 1);
                }
                printf("Recieved %d packets(including termination packet)\n", i-1);
                printf("Saving the file\n");
                close(file_fd);
                printf("File %s created\n", buffer);
                
            }
        }

    }
    

}
