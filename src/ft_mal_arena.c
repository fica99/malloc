/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_mal_arena.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aashara- <aashara-@student.21-school.ru    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 16:23:18 by aashara-          #+#    #+#             */
/*   Updated: 2021/10/21 16:23:18 by aashara-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_mal_precomp.h"
#include "ft_mal_arena.h"
#include "ft_mal_heap.h"

static t_s_ft_mal_state	*ft_mal_new_arena(size_t alloc_size)
{
	t_s_ft_mal_state		*arena;
	t_s_ft_mal_heap_info	*heap_info;

	heap_info = ft_mal_new_heap(alloc_size + FT_MAL_STATE_SIZE + FT_MAL_HEAP_INFO_SIZE);
	
	// error check
	if (!heap_info)
		return (NULL);
	
	arena = FT_MAL_HEAP_INFO_SHIFT(heap_info);
	// check mutex initialization
	if (pthread_mutex_init(&arena->mutex, NULL) != FT_MAL_SUCCESS)
	{
//		ft_mal_free_heap(heap_info);
		return (NULL);
	}
	arena->next = NULL;
	return (arena);
}

static bool				ft_mal_is_available_arena(t_s_ft_mal_state *arena)
{
	if (!arena)
		return (false);
	if (FT_MAL_ARENA_TRYLOCK(&arena->mutex) == FT_MAL_SUCCESS)
		return (true);
	return (false);
}

t_s_ft_mal_state		*ft_mal_get_available_arena(size_t alloc_size)
{
	static t_s_ft_mal_state	*arena;
	t_s_ft_mal_state		*res_arena;

	res_arena = arena;
	while (true)
	{
		// check limit of arenas creation
		if (!res_arena && (!arena || arena->arena_id < FT_MAL_MAX_NB_ARENAS))
		{
			// create new arena
			res_arena = ft_mal_new_arena(alloc_size);

			// error check
			if (!res_arena)
				return (NULL);

			res_arena->next = arena;
			arena = res_arena;
			arena->arena_id = (arena->next ? arena->next->arena_id + 1 : 0);
		}
		
		// continue loop from the beginning if all arenas are locked
		if (!res_arena)
			res_arena = arena;
		else
		{
			if (ft_mal_is_available_arena(res_arena))
				break ;
			res_arena = res_arena->next;
		}
	}
	return (res_arena);
}
