#include "include/ft_mal_memory.h"
#include <assert.h>
#include <stdio.h>
int	main(void)
{
	int *a = malloc(0);
	free(a);
	a = malloc(100);
	//printf("Hello %ls\n", a);
	free(a);
	a = malloc(1000);
	free(a);
	a = malloc(100000);
	free(a);
	return (0);
}
