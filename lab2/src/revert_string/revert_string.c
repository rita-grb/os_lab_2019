#include "revert_string.h"
#include "string.h"

extern void RevertString(char *str)
{
	int middle = strlen(str)/2;
    for (int i = 0; i < middle; ++i) 
	{
		char temp = str[i];
		str[i] = str[strlen(str) - i - 1];
		str[strlen(str) - i - 1] = temp;
    }
}

