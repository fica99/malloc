#include <stdio.h>
#include "ft_mal_memory.h"

#include "libft.h"

int	main(void)
{
	void	*a = malloc(1024);

	ft_bzero(a, 100);
	free(a);
	// int *a = malloc(0);
	// printf("Hello %ls\n", a);
	// free(a);
	// a = malloc(100);
	// printf("Hello %ls\n", a);
	// free(a);
	// a = malloc(1000);
	// printf("Hello %ls\n", a);
	// free(a);
	// a = malloc(100000);
	// printf("Hello %ls\n", a);
	// free(a);


	return (0);
}
