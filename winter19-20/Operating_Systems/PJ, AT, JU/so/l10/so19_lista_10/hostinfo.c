#include "csapp.h"

int main(int argc, char **argv) {
  struct addrinfo *p, *listp, hints;
  char buf[MAXLINE];
  int rc, flags;

  if (argc != 3)
    app_error("usage: %s <domain name> <service>\n", argv[0]);

  /* Get a list of addrinfo records */
  memset(&hints, 0, sizeof(struct addrinfo));
  /* --- set to AF_UNSPEC because we want both of AF_INET and AF_INET6 */
  hints.ai_family = AF_UNSPEC; /* IPv4 only */
  /* --- set to 0 because of tftp */
  hints.ai_socktype = 0;
  /* Connections only */

  /* --- tftp is defined as udp service in /etc/services 69/tftp
   * but we set socktype to SOCK_STREAM aka TCP so we must remove hint :) */
  if ((rc = getaddrinfo(argv[1], argv[2], &hints, &listp)) != 0)
    gai_error(rc, "getaddrinfo");

  /* Walk the list and display each IP address */
  char port[32];
  flags = NI_NUMERICHOST | NI_NUMERICSERV; /* Display address string instead of domain name */
  for (p = listp; p; p = p->ai_next) {
    Getnameinfo(p->ai_addr, p->ai_addrlen, buf, MAXLINE, port, 32, flags);
    printf("%s:[%s]\n", buf, port);
  }

  /* Clean up */
  freeaddrinfo(listp);

  return EXIT_SUCCESS;
}
