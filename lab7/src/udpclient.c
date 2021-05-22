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
  char* addr = NULL;

   while (1)
  {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"bufsize", required_argument, 0, 0},
                                      {"port", required_argument, 0, 0},
                                      {"addr", required_argument, 0, 0},
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

          case 2:
          {
            addr = (char*)malloc(strlen(optarg) * sizeof(char));
            memcpy(addr, optarg, strlen(optarg) * sizeof(char));
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

  if (bufsize == -1 || port == -1 || addr == NULL)
  {
    printf("\nUsage: %s --bufsize \"num\" --port \"num\" --addr \"num\" \n\n", argv[0]);
    return 1;
  }
  
  int sockfd, n;
  char sendline[bufsize], recvline[bufsize + 1];
  struct sockaddr_in servaddr;
  struct sockaddr_in cliaddr;

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);

  if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) < 0)
  {
    perror("\ninet_pton problem\n\n");
    exit(1);
  }
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    perror("\nsocket problem\n\n");
    exit(1);
  }

  write(1, "Enter string\n", 13);

  while ((n = read(0, sendline, bufsize)) > 0)
  {
    if (sendto(sockfd, sendline, n, 0, (SADDR*)&servaddr, SLEN) == -1)
    {
      perror("\nsendto problem\n\n");
      exit(1);
    }

    if (recvfrom(sockfd, recvline, bufsize, 0, NULL, NULL) == -1)
    {
      perror("\nrecvfrom problem\n\n");
      exit(1);
    }

    printf("\nREPLY FROM SERVER= %s\n\n", recvline);
  }
  close(sockfd);
}

