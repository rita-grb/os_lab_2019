#include "swap.h"

void Swap(char *left, char *right)
{
	char param=*left;
	*left=*right;
	*right=param;
}
