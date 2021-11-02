/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_reallocf.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aashara- <aashara-@student.21-school.ru    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 15:58:58 by aashara-          #+#    #+#             */
/*   Updated: 2021/11/02 18:40:13 by aashara-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
*************************** User headers ************************************
*/

/*
*********** Precompiled header **************
*/
#include "ft_mal_precomp.h"

/*
*********** All memory functions header ********************
*/
#include "ft_mal_memory.h"

/*
*********** Arena header ********************
*/
#include "ft_mal_arena.h"

// same as realloc, but if there is an error, it frees ptr
void	*reallocf(void *ptr, size_t size)
{
	t_s_ft_mal_state	*arena;
	void				*res;
	
	// if there is no ptr, allocate new pointer
	if (ptr == NULL)
		return (malloc(size));

	// if size is 0, free pointer and return NULL
	if (!size)
	{
		free(ptr);
		return (malloc(0));
	}

	// get arena by ptr (locking mutex)(first search in current thread arena, and then in others)
	arena = ft_mal_get_arena_by_ptr(ptr);

	// no one arena allocated this memory (memory was not allocated error)
	if (!arena)
	{
		// if there is error return NULL
		return (NULL);
	}

	// realloc memory by ptr
	res = ft_mal_realloc_memory(arena, ptr, size);

	// error occured
	if (!res)
		ft_mal_free_memory(arena, ptr);

	// unlock mutex after reallocating the memory
	FT_MAL_MUTEX_UNLOCK(&arena->mutex);

	return (res);
}
