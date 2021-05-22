
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

bool is_timeout_ends = false;

void handlr(int sig)
{
    printf("Timeout is over.\n");
    is_timeout_ends = true;
}

int genocyde(pid_t* children, int quant)
{
    for(int i = 0;i < quant;i++)
    {
        kill(children[i], SIGKILL);
        printf("Killing a child process %d\n",children[i]);
    }

    return -1;
}

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  bool with_files = false;
  int timeout = -1;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"timeout", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            // your code here

           if(seed <= 0)
           {
               printf("Seed is a positive number");
               seed = -1;
           }

            // error handling
            break;
          case 1:
            array_size = atoi(optarg);
            // your code here

            if(array_size <= 0)
           {
               printf("Array size is a positive number");
               array_size = -1;
           }

            // error handling
            break;
          case 2:
            pnum = atoi(optarg);
            // your code here

            if(pnum <= 0 || pnum > array_size)
           {
               printf("Array size is a positive number less then array size");
               array_size = -1;
           }

            // error handling
            break;
          case 3:
            timeout = atoi(optarg);

            if(timeout <= 0)
           {
               printf("Timeout is a positive number");
               timeout = -1;
           }

            break;
         case 4:
            with_files = true;
            break;

          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;

      case 'f':
        with_files = true;
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  int *array = (int*)malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  FILE* fp;
  int** fd;

  if(timeout > 0)
  {
      signal(SIGALRM, handlr);
      alarm(timeout);
  }

  if(with_files)
  {
    fp = fopen("fake_pipe.txt","w");
  }
  else 
  {
    fd = (int**)malloc(pnum*sizeof(int*));
    for(int i = 0; i < pnum; i++)
    {
        fd[i] = (int*)malloc(2 * sizeof(int));
        if(pipe(fd[i]) == -1)
        {
            printf("Failing in making pipe.");
            return 1;
        }
    }
  }

  int step = array_size/pnum;
  int* children = (int*)calloc(pnum,sizeof(pid_t));

  for (int i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    children[i] = child_pid;

    if (child_pid >= 0) {
      // successful fork
      active_child_processes += 1;
      if (child_pid == 0) {
        // child process

        struct MinMax cur;

        if(i < pnum - 1)
        {
            cur = GetMinMax(array, i*step, (i+1)*step);
        }
        else
        {
            cur = GetMinMax(array, i*step, array_size);
        }

        // parallel somehow

        if (with_files) 
        {
          fwrite(&cur,sizeof(struct MinMax),1,fp);
        } 
        else 
        {
          write(fd[i][1],&cur,sizeof(struct MinMax));
          close(fd[i][1]);
        }
        return 0;
      }

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  if(with_files)
  fclose(fp);

  int status;

  while (active_child_processes > 0) {
    if(is_timeout_ends)
        return genocyde(children,pnum); //вызов kill для дочерних процессов, со включенным таймером

     if (waitpid(-1, &status, WNOHANG) > 0) //ожидание завершения работы
        {
            active_child_processes -= 1;
        }
      wait(NULL);
    active_child_processes -= 1;
  }

  struct MinMax min_max,buff;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;
  fp = fopen("fake_pipe.txt","r");

  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;

    if (with_files) {
      fread(&buff,sizeof(struct MinMax),1,fp);
      min = buff.min;
      max = buff.max;
    } 
    else 
    {
      read(fd[i][0], &buff, sizeof(struct MinMax));
      min = buff.min;
      max = buff.max;
    }

    if (min < min_max.min) min_max.min = min;
    if (max > min_max.max) min_max.max = max;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}