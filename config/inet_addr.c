#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
int main(int argc, char **argv)
{
  struct in_addr ia;
  ia.s_addr = inet_addr("0.0.0.0");
  if (argc == 2)
  {
    if (strcmp("safamily",argv[1]) == 0)
      printf("%i\n",(int)sizeof(sa_family_t));
    if (strcmp("sockaddr",argv[1]) == 0)
      printf("%i\n",(int)sizeof(struct sockaddr_in));
    if (strcmp("sockaddrv6",argv[1]) == 0)
      printf("%i\n",(int)sizeof(struct sockaddr_in6));
  }
  return(0);
}

