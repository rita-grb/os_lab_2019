#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

bool fkill = false;

void alarm_handler (int signum)
{
  fkill = true;
}

int main(int argc, char **argv)
{
  int seed = -1;
  int array_size = -1;
  int pnum = -1; // кол-во процессов
  int timeout = -1;
  bool with_files = false;
    
  while (true)
  {
    /* Индекс на следующий указатель argv, который будет обработан при следующем вызове getopt() */
    int current_optind = optind ? optind : 1; 

    static const struct option options[] = {{"seed", required_argument, 0, 0},
                                            {"array_size", required_argument, 0, 0},
                                            {"pnum", required_argument, 0, 0},
                                            {"timeout", required_argument, 0, 0},
                                            {"by_files", no_argument, 0, 'f'},
                                            {0, 0, 0, 0}};

    int option_index = 0; // индекс опции
      
    /* getopt_long возвращает первый параметр и задает некоторые глобальные переменные.
     * При повторном вызове с теми же аргументами функция вернет следующий параметр и задаст 
     * глобальную переменную. Если больше не будет найдено опций, которые надо распознать, то функция
     * вернет -1, что означает завершение обработки команды.
     */
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) // опций для обработки больше нет, завершение обработки команды
    break;

    switch (c)
    {
      case 0:
      {
        switch (option_index) 
        {
          case 0:
          {
            seed = atoi(optarg);
            if(seed <= 0)
            {
              printf("\nSeed is a positive number!\n\n");
              return 1;
            }
          }  
          break;

          case 1:
          {
            array_size = atoi(optarg);
            if (array_size <= 0)
            {
              printf("\nArray_size is a positive number!\n\n");
              return 1;
            }
          }
          break;

          case 2:
          {
            pnum = atoi(optarg);
            if(pnum <= 0)
            {
              printf("\nPnum is a positive number!\n\n");
              return 1;
            }  
          }
          break;

          case 3:
          {
            timeout = atoi(optarg);
            if (timeout <= 0)
            {
              printf("\nTimeout is a positive number!\n\n");
              return 1;
            }
          }
          break;

          case 4:
          with_files = true;
          break;

          default:
          printf("\nIndex %d is out of options\n\n", option_index);
        }
      }
      break;
            
      case 'f':
      with_files = true;
      break;

      case '?':
      break;

      default:
      printf("\ngetopt returned character code 0%o?\n\n", c);
    }
  }

  if (optind < argc)
  {
    printf("\nHas at least one no option argument\n\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1 || timeout == -1)
  {
    printf("\nUsage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n\n", argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0; // кол-во активных дочерних процессов

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  int step = array_size / pnum;

  FILE* fl;  // создаем указатель на файл
  int fd[2]; // создаем указатель на массив из двух целочисленных переменных

  if(with_files)
  {
    // Попытаемся открыть файл
    if((fl = fopen("usage_file.txt", "w")) == NULL)
    {
      printf("Can\'t open file\n");
      return 1;
    }
  }
  else
  {
    // Попытаемся создать pipe
    if(pipe(fd) < 0)
    {
      printf("Can\'t create pipe\n");
      return 1;
    }
  }
  
  if (timeout != -1)
  {
    signal(SIGALRM, alarm_handler);
    alarm(timeout);
  }

  pid_t children[pnum];

  for (int i = 0; i < pnum; i++) 
  {
    pid_t child_pid = fork(); // создаем дочерний процесс

    if (child_pid >= 0) 
    {
      active_child_processes += 1;
      if (child_pid == 0) // мы находимся в дочернем процессе
      {
        struct MinMax min_max_i;
                
        if (i != pnum - 1)
        {
          min_max_i = GetMinMax(array, i * step, (i + 1) * step - 1);
        }
        else
        {
          min_max_i = GetMinMax(array, i * step, array_size);
        }

        if (with_files) 
        {
          fwrite(&min_max_i.min, sizeof(int), 1, fl);
          fwrite(&min_max_i.max, sizeof(int), 1, fl);
          fclose(fl);
        } 
        else 
        {
          close(fd[0]);
          write(fd[1], &min_max_i.min, sizeof(int));
          write(fd[1], &min_max_i.max, sizeof(int));
          close(fd[1]);
        }
        return 0;
      }
      if (child_pid > 0)
      {
        children[i] = child_pid;
      }
    }

    int status;

    while (active_child_processes > 0)
    {
      if (fkill == true)
      {
        printf("\nKilling a child process...\n");
        for (int i = 0; i < pnum; ++i)
        {
          kill(children[i], SIGKILL);
        }
        return 1;
      }

      if (waitpid(-1, &status, WNOHANG) > 0)
      {
        active_child_processes -= 1;
      }      
    }

    struct MinMax min_max;
    min_max.min = INT_MAX;
    min_max.max = INT_MIN;

    for (int i = 0; i < pnum; i++)
    {
      int min = INT_MAX;
      int max = INT_MIN;

      if (with_files)
      {
        fl = fopen("usage_file.txt", "r");
        fread(&min, sizeof(int),1, fl);
        fread(&max, sizeof(int),1, fl);
      }
      else 
      {
        close(fd[1]);
        read(fd[0], &min, sizeof(int));
        read(fd[0], &max, sizeof(int));
        close(fd[0]);
      }

      if (min < min_max.min)
      min_max.min = min;
            
      if (max > min_max.max)
      min_max.max = max;
    }
  
    if (with_files) 
    {
      fclose(fl);
    }
    
    struct timeval finish_time;
    gettimeofday(&finish_time, NULL);

    double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
    elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;
    free(array);

    printf("\nMin: %d\n", min_max.min);
    printf("Max: %d\n", min_max.max);
    printf("Elapsed time: %fms\n\n", elapsed_time);
    fflush(NULL);
    return 0;
  }
}
