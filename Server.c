
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
    int sockfd, newSockfd;
    struct sockaddr_in address;
    int addrLength= sizeof(address);

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


    if(listen(sockfd, MAX_CLIENT)<0) {
        perror("error on listnining incoming connection");
        exit(EXIT_FAILURE);
    };


    printf("Listning on port %d \n", PORT);
    newSockfd=accept(sockfd,(struct sockaddr *)&address, (socklen_t *)&addrLength);

    if(newSockfd <0) {
        perror("Error on accepting clients ");
        exit(EXIT_FAILURE);
    }


    close(newSockfd);
    close(sockfd);
    return 0;
}
