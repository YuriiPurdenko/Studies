#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>


#define ERROR(str) { fprintf(stderr, "%s: %s\n", str, strerror(errno)); exit(EXIT_FAILURE); }
#define MIN(a,b) (((a)<(b))?(a):(b))
#define BUFFER_SIZE 10000000	// 10 MB

u_int8_t recv_buffer[BUFFER_SIZE+1];


// Funkcja recv_till_separator czyta z gniazda, czekajac co najwyzej timeout sekund
// albo do wystapienia w przeczytanym ciagu znaku separatora.
//
// Zwraca -2 jeśli wystąpił timeout
// Zwraca -1 jeśli wystąpił błąd odczytu z gniazda (errno zawiera kod błędu)
// Zwraca 0 jeśli nadawca zamknął połączenie nie wysławszy znaku separatora.
// Zwraca liczbę przeczytanych bajtów zawierających separator w przeciwnym przypadku.
//
// Uwaga: funkcja moze przeczytac wiecej niz do znaku separatora i te dane zostana zwrocone
// i usuniete z bufora odbiorczego. Kolejne wywolania recv_till_separator() / recv() nie
// zwroca juz tych danych.

ssize_t recv_till_separator (int fd, u_int8_t *buffer, size_t buffer_size, unsigned int timeout, char separator)
{
	struct timeval tv; tv.tv_sec = timeout; tv.tv_usec = 0;
	bool separator_found = false;
	size_t total_bytes_read = 0;
	while (total_bytes_read < buffer_size) {
		printf("Current value of tv = %.3f\n", (double)tv.tv_sec + (double)tv.tv_usec / 1000000);

		fd_set descriptors;
		FD_ZERO(&descriptors);
		FD_SET(fd, &descriptors);
		int ready = select(fd + 1, &descriptors, NULL, NULL, &tv);
		if (ready < 0)
			ERROR("select error");
		if (ready == 0)
			return -2;

		ssize_t bytes_read = recv(fd, buffer + total_bytes_read, buffer_size - total_bytes_read, 0);
		if (bytes_read <= 0)
			return bytes_read;

		for (int i = 0; i < bytes_read; i++) {
			if (buffer[total_bytes_read + i] == separator)
				separator_found = true;
		}
		total_bytes_read += bytes_read;

		if (separator_found)
			return total_bytes_read;
	}
	return 0;
}


void send_all (int sockfd, u_int8_t* buffer, size_t n)
{
	size_t n_left = n;
	while (n_left > 0) {
		ssize_t bytes_sent = send(sockfd, buffer, n_left, 0);
		if (bytes_sent < 0)
		ERROR("send error");
		printf("%ld bytes sent\n", bytes_sent);
		n_left -= bytes_sent;
		buffer += bytes_sent;
	}
}


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

		// Czekamy max. 10 sekund na ciąg bajtów zawierający znak 'X'.
		ssize_t bytes_read = recv_till_separator(connected_sockfd, recv_buffer, BUFFER_SIZE, 10, 'X');
		if (bytes_read == -2) {
			printf ("Timeout\n");
		} else if (bytes_read == -1) {
			ERROR("recv error");
		} else if (bytes_read == 0) {
			printf ("Client closed connection");
		} else {
			recv_buffer[bytes_read] = 0;
			printf ("Data received: '%s'\n", recv_buffer);
			send_all (connected_sockfd, recv_buffer, bytes_read);
		}

		if (close (connected_sockfd) < 0)
			ERROR("close error");
	}
}

