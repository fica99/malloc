/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_realloc.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aashara- <aashara-@student.21-school.ru    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 15:58:58 by aashara-          #+#    #+#             */
/*   Updated: 2021/10/31 23:32:32 by aashara-         ###   ########.fr       */
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


void	*realloc(void *ptr, size_t size)
{
	t_s_ft_mal_state	*arena;
	
	// if there is no ptr, allocate new pointer
	if (ptr == NULL)
		return (malloc(size));

	// if size is 0, free pointer and return NULL
	if (!size)
	{
		free(ptr);
		return (NULL);
	}

	// get arena by ptr (locking mutex)(first search in current thread arena, and then in others)
	arena = ft_mal_get_arena_by_ptr(ptr);

	// no one arena allocated this memory (memory was not allocated error)
	if (!arena)
		return (ptr);

	// realloc memory by ptr
	ptr = ft_mal_realloc_memory(arena, ptr, size);

	// unlock mutex after reallocating the memory
	FT_MAL_MUTEX_UNLOCK(&arena->mutex);

	return (ptr);
}
