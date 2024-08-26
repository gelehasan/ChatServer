#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


#define  PORT 8080
#define MAX_CLIENT 10
int main(void)
{
    int sockfd, newSockfd, activity, bytes_read, client_socket[MAX_CLIENT],MAX_SD, current_client;
    struct sockaddr_in address, client_address;
    int addrLength= sizeof(address);
    fd_set reads_fds;
    char buffer[255];
    
    // setting current socket being monitored to zero
    for(int i=0; i<MAX_CLIENT; i++) {
        client_socket[i]=0;
    }
    sockfd=socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        perror( "Error on establishing socket");
        exit(EXIT_FAILURE);
    }
    memset(&address, 0, addrLength);
    address.sin_family=AF_INET;
    address.sin_addr.s_addr=INADDR_ANY;
    address.sin_port= htons(PORT);

    if(bind(sockfd,(const struct sockaddr *)&address, sizeof(address))<0) {
        perror("Error on binding connection");
        exit(EXIT_FAILURE);
    }


    if(listen(sockfd, 3)<0) {
        perror("error on listnining incoming connection");
        exit(EXIT_FAILURE);
        close(sockfd);
    };

    while(1) {
        // reseting monitored socket
        FD_ZERO(&reads_fds);
        FD_SET(sockfd,&reads_fds);
        MAX_SD=sockfd;

        //Checking if there is already connected client and adding them to the monitoring listttttttttttttttttt
        for(int i=0; i<MAX_CLIENT; i++) {
            current_client= client_socket[i];

            if(current_client >0) {
                FD_SET(current_client, &reads_fds);
            }

            if(current_client > MAX_SD) {
                MAX_SD=current_client;
            }
        }


        // Monitiring activities
        activity= select(MAX_SD+1, &reads_fds, NULL, NULL,NULL);

        if(activity <0  ) {
            printf("Error on selct");
        }


        // Monitoring incomming calls
        if(FD_ISSET(sockfd,&reads_fds)) {
            newSockfd= accept(sockfd,(const struct sockaddr*)&address, sizeof(address));
            if(newSockfd <0) {
                perror("Error on accepting new incomming call");
                exit(EXIT_FAILURE);
            }

           for(int i=0; i<MAX_CLIENT; i++) {
               current_client= client_socket[i];

               if(current_client==0) {
                   current_client=newSockfd;
                   FD_SET(current_client, &reads_fds);
                   break;
               }
           }
        }


        // Monitoring IO operations

        for(int i=0; i<MAX_CLIENT; i++) {
            current_client= client_socket[i];

           if(FD_ISSET(current_client,&reads_fds)) {

               bytes_read= read(current_client, buffer, 255);

               //If less than zero it means clients disconnected
               if(bytes_read <0) {

                   getpeername(current_client,(struct sockadd*)&client_address, (socklen_t *)&addrLength);

                   // inet_ntoa converts IP4 address from binary to into its standard dotted-decimal string representation.,
                   // ntohs converts from network byte to host byte
                   printf("Disconnected IP: %s Port %d \n",inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
                   close(current_client);
                   current_client=0;
               }else {

                   buffer[bytes_read]="\0"; // Null terminator
                   send(current_client, buffer, strlen(buffer),0);
               }

           }
        }
    }

    return 0;
}
