#include <arpa/inet.h>
#include <getopt.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define SADDR struct sockaddr
#define SLEN sizeof(struct sockaddr_in)

int main(int argc, char** argv) 
{
  int bufsize = -1;
  int port = -1;

  while (1)
  {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"bufsize", required_argument, 0, 0},
                                      {"port", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);

    if (c == -1)
    break;

    switch (c)
    {
      case 0:
      {
        switch (option_index)
        {
          case 0:
          {
            bufsize = atoi(optarg);
            if (bufsize < 0)
            {
              printf("\nThe bufsize is a positive number\n\n");
              return 1;
            }
          }
          break;

          case 1:
          {
            port = atoi(optarg);
            if (port <= 0)
            {
              printf("\nThe port is a positive number\n\n");
              return 1;
            }
          }
          break;

          default:
          printf("\nIndex %d is out of options\n\n", option_index);
        }
      }
      break;

      case '?':
      {
        printf("\nArguments error\n\n");
      }
      break;

      default:
      fprintf(stderr, "\ngetopt returned character code 0%o?\n\n", c);
    }
  }

  if (optind < argc)
  {
    printf("\nHas at least one no option argument\n\n");
    return 1;
  }

  if (bufsize == -1 || port == -1)
  {
    printf("\nUsage: %s --bufsize \"num\" --port \"num\" \n\n", argv[0]);
    return 1;
  }

  int sockfd, n;
  char mesg[bufsize], ipadr[16];
  struct sockaddr_in servaddr;
  struct sockaddr_in cliaddr;

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    perror("\nsocket problem\n\n");
    exit(1);
  }

  memset(&servaddr, 0, SLEN);
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(port);

  if (bind(sockfd, (SADDR*)&servaddr, SLEN) < 0)
  {
    perror("\nbind problem\n\n");
    exit(1);
  }
  printf("\nSERVER starts...\n\n");

  while (1)
  {
    unsigned int len = SLEN;

    if ((n = recvfrom(sockfd, mesg, bufsize, 0, (SADDR *)&cliaddr, &len)) < 0)
    {
      perror("\nrecvfrom\n\n");
      exit(1);
    }
    mesg[n] = 0;

    printf("\nREQUEST %s\nFROM %s : %d\n\n", mesg, inet_ntop(AF_INET, (void*)&cliaddr.sin_addr.s_addr, ipadr, 16), ntohs(cliaddr.sin_port));

    if (sendto(sockfd, mesg, n, 0, (SADDR *)&cliaddr, len) < 0)
    {
      perror("\nsendto\n\n");
      exit(1);
    }
  }
}
