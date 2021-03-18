#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define ERROR(str) { fprintf(stderr, "%s: %s\n", str, strerror(errno)); exit(EXIT_FAILURE); }
#define MIN(a,b) (((a)<(b))?(a):(b))
#define BUFFER_SIZE 10000000	// 10 MB

u_int8_t recv_buffer[BUFFER_SIZE+1];


int main()
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);	
	if (sockfd < 0)
		ERROR("socket error");

	struct sockaddr_in server_address;
	bzero (&server_address, sizeof(server_address));
	server_address.sin_family      = AF_INET;
	server_address.sin_port        = htons(32345);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind (sockfd, (struct sockaddr*) &server_address, sizeof(server_address)) < 0)
		ERROR("bind error");

	if (listen (sockfd, 64) < 0)
		ERROR("listen error");

	for (;;) {

		int connected_sockfd = accept (sockfd, NULL, NULL);
		if (connected_sockfd < 0)
			ERROR("accept error")

		ssize_t bytes_read = recv(connected_sockfd, recv_buffer, BUFFER_SIZE, 0);
		if (bytes_read < 0)
			ERROR("recv error");

		recv_buffer[MIN(bytes_read, 20)] = 0;
		char reply_msg[100];
		sprintf (reply_msg, "%ld bytes read, first 20 bytes: '%s'", bytes_read, recv_buffer);
		printf ("%s\n", reply_msg);

		size_t reply_msg_len = strlen(reply_msg);
		ssize_t bytes_sent = send (connected_sockfd, reply_msg, reply_msg_len, 0);
		if (bytes_sent < 0)
			ERROR("send error");

		if (close (connected_sockfd) < 0)
			ERROR("close error");
	}
}
