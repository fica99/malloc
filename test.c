#include "include/ft_mal_memory.h"
#include <assert.h>
#include <stdio.h>
int	main(void)
{
	int *a = malloc(100);
	//printf("Hello %ls\n", a);
	free(a);
	return (0);
}
