#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>

int main (int argc, char* argv[])
{
	if (argc != 2) {
		fprintf (stderr, "Usage: %s domain_name\n", argv[0]);
		return EXIT_FAILURE;
	}
	
	struct addrinfo* result;
	struct addrinfo hints = {
		.ai_family = AF_INET,			// chcemy tylko adresy IPv4
		.ai_socktype = SOCK_STREAM,		// inaczej dostaniemy trzy struktury (dla SOCK_STREAM, SOCK_DGRAM i SOCK_RAW)
	};

	int error = getaddrinfo (argv[1], NULL, &hints, &result);
	if (error != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", (error == EAI_SYSTEM) ? strerror(errno) : "other error");
		return EXIT_FAILURE;
	}

	for (struct addrinfo* r = result; r != NULL; r = r->ai_next) { 
		struct sockaddr_in* addr = (struct sockaddr_in*)(r->ai_addr);
		char ip_address[20];
		inet_ntop (AF_INET, &(addr->sin_addr), ip_address, sizeof(ip_address));
		printf ("%s\n", ip_address);
	}

	freeaddrinfo (result);
	return EXIT_SUCCESS;
}
