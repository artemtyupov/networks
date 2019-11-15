#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_MSG_SIZE 256
#define PORT 3000

#define esc 27

void start_chat(int sockfd) {

    char buff[MAX_MSG_SIZE];
    int n, length;
    int max_msg_length = 0;
    struct sockaddr_in client_addr;
    length = sizeof(client_addr);

    for(;;) {
        bzero(buff, sizeof(buff));

        recvfrom(sockfd, buff, sizeof(buff), 0, (struct sockaddr*)&client_addr, &length);

        if (*buff == esc) {
            printf("> Client disconnected.\n");
            printf("> Max message length: %d\n", max_msg_length);
            max_msg_length = 0;
            bzero(buff, MAX_MSG_SIZE);

            continue;
        }

        printf("\nFrom client: %s\nTo client: ", buff);

        int i = strlen(buff);
        if (i > max_msg_length) {
            max_msg_length = i;
        }

        bzero(buff, sizeof(buff));

        n = 0;
        while ((buff[n] = getchar()) != '\n') {
            n++;
        }

        //if (n > max_msg_length) {
        //    max_msg_length = n;
        //}

        sendto(sockfd, buff, sizeof(buff), 0, (struct sockaddr*)&client_addr, length);
    }
}

int main() {

    int sockfd;
    struct sockaddr_in servaddr;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd == -1) {
        printf("> Socket creation failed.\n");
        exit(0);
    } else {
        printf("> Socket successfully created.\n");
    }

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if((bind(sockfd,(struct sockaddr*) &servaddr, sizeof(servaddr))) != 0) {
        printf("> Socket bind failed.\n");
        exit(0);
    } else {
        printf("> Socket successfully binded.\n");
    }

    start_chat(sockfd);
    close(sockfd);
}