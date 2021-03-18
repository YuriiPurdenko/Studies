#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#define ERROR(str) { fprintf(stderr, "%s: %s\n", str, strerror(errno)); exit(EXIT_FAILURE); }
#define BUFFER_SIZE 4096


int main (int argc, char* argv[])
{
	int N;
	if (argc != 2) { 
		fprintf(stderr, "Usage: client <nbytes>\n"); 
		exit(EXIT_FAILURE); 
	} else {
		N = atoi(argv[1]);
	}

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);	
	if (sockfd < 0) 
		ERROR("socket error")

	struct sockaddr_in server_address;
	bzero (&server_address, sizeof(server_address));
	server_address.sin_family	= AF_INET;
	server_address.sin_port		= htons(32345);
	inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);
	if (connect (sockfd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
		ERROR("connect error");

	char sending_buffer[N+1];
	for (int i = 0; i < N; i++)
		sending_buffer[i] = (char)('0' + i % 10);
	ssize_t bytes_sent = send (sockfd, sending_buffer, N, 0);
	printf("%ld bytes sent\n", bytes_sent);
	if (bytes_sent < N)
		ERROR("send error");		// Czy to faktycznie jest blad?

	u_int8_t recv_buffer[BUFFER_SIZE+1];
	ssize_t bytes_read = recv (sockfd, recv_buffer, BUFFER_SIZE, 0);
	if (bytes_read < 0)
		ERROR("recv error");

	recv_buffer[bytes_read] = 0;
	printf ("server reply: %s\n", recv_buffer);
	
	if (close (sockfd) < 0)
		ERROR("close error");
}
