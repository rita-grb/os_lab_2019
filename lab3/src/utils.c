#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
void GenerateArray(int *array, unsigned int array_size, unsigned int seed) 
{
  srand(seed);
  for (int i = 0; i < array_size; i++) 
  {
    array[i] = seed+rand()%100;
    printf("%d ",array[i]);
  }
  printf("\n");
}
