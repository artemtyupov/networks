#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_MSG_SIZE 256
#define PORT 3000

#define esc 27

int main() {

	char buff[MAX_MSG_SIZE];
	int sockfd,len,n;
	struct sockaddr_in servaddr;
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	if(sockfd == -1) {
		printf("> Socket creation failed.\n");
		exit(0);
	} else {
		printf("> Socket successfully created.\n");
	}

	bzero(&servaddr, sizeof(len));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);
	len = sizeof(servaddr);

	for(;;) {
		printf("To server: ");

		scanf("%s", buff);

		sendto(sockfd, buff, sizeof(buff), 0, (struct sockaddr*)&servaddr, len);

		if (*buff == esc) {
			printf("> Client Exit.\n");
			bzero(buff, sizeof(buff));
			break;
		}

		bzero(buff, sizeof(buff));
		n = recvfrom(sockfd, buff, sizeof(buff), 0, (struct sockaddr*)&servaddr, &len);

		printf("From Server: %s\n", buff);
		bzero(buff, sizeof(buff));
	}
	close(sockfd);
}