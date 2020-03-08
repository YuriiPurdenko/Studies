#include "csapp.h"

int main(int argc, char **argv) {
  struct addrinfo *p, *listp, hints;
  char buf[MAXLINE + 1], buf2[MAXLINE + 1];
  int rc, flags;
  
  if (argc < 2 || argc > 3)
    app_error("usage: %s <domain name> *<port> \n", argv[0]);

  /* Get a list of addrinfo records */
  memset(&hints, 0, sizeof(struct addrinfo));
  //hints.ai_family = AF_INET; /* IPv4 only */
  hints.ai_family = AF_UNSPEC;
  //hints.ai_socktype = 0;
  hints.ai_socktype = SOCK_STREAM;
  /* Connections only */
  if(argc == 3){  
    if ((rc = getaddrinfo(argv[1], argv[2], &hints, &listp)) != 0)
      gai_error(rc, "getaddrinfo");
    flags = NI_NUMERICHOST|NI_NUMERICSERV; /* Display address string instead of domain name */
    for (p = listp; p; p = p->ai_next) {
        Getnameinfo(p->ai_addr, p->ai_addrlen, buf, MAXLINE, buf2, MAXLINE, flags);
        if(p->ai_family == AF_INET)
            printf("%s:%s\n", buf,buf2);
        else
            printf("[%s]:%s\n", buf,buf2);
    }
  }
  else{
    if ((rc = getaddrinfo(argv[1], NULL, &hints, &listp)) != 0)
      gai_error(rc, "getaddrinfo");
    flags = NI_NUMERICHOST; /* Display address string instead of domain name */
    for (p = listp; p; p = p->ai_next) {
        Getnameinfo(p->ai_addr, p->ai_addrlen, buf, MAXLINE, NULL, 0, flags);
        if(p->ai_family == AF_INET)
            printf("%s\n", buf);
        else
            printf("[%s]\n", buf);
    }   
  }

  freeaddrinfo(listp);

  return EXIT_SUCCESS;
}   
