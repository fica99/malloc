/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_malloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aashara- <aashara-@student.21-school.ru    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 15:58:58 by aashara-          #+#    #+#             */
/*   Updated: 2021/10/21 15:58:58 by aashara-         ###   ########.fr       */
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


void	*malloc(size_t size)
{
	t_s_ft_mal_state	*arena;
	void				*ptr;

	// get available arena (locking mutex)
	arena = ft_mal_get_available_arena();

	// error happened
	if (!arena)
		return (NULL);

	ptr = ft_mal_allocate_memory_from_arena(arena, size);

	// unlock mutex after allocating the memory
	FT_MAL_MUTEX_UNLOCK(&arena->mutex);

	return (ptr);	
}
