/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_free.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aashara- <aashara-@student.21-school.ru    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 15:58:58 by aashara-          #+#    #+#             */
/*   Updated: 2021/10/30 16:20:08 by aashara-         ###   ########.fr       */
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


void		free(void *ptr)
{
	t_s_ft_mal_state	*arena;
	
	// empty pointer
	if (ptr == NULL)
		return ;
		
	// get saved arena for current thread (locking mutex)
	arena = ft_mal_get_saved_arena();

	// no saved arena for current thread (memory was not allocated error)
	if (!arena)
		return ;

	// free ptr to arena
	ft_mal_free_memory(arena, ptr);

	// unlock mutex after allocating the memory
	FT_MAL_MUTEX_UNLOCK(&arena->mutex);
	
}
