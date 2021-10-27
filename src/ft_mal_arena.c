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


/*
*************************** User headers ************************************
*/

/*
*********** Precompiled header **************
*/
#include "ft_mal_precomp.h"

/*
*********** Arena header ********************
*/
#include "ft_mal_arena.h"

/*
*********** Heap header *********************
*/
#include "ft_mal_heap.h"


/*
*************************** System headers ************************************
*/

/*
*********** gettid **************************
*********** syscall *************************
*/
#define _GNU_SOURCE
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>


/*
*************************** Macroses  *****************************************
*/

/*
*********** Limits ***************************
*/
# define FT_MAL_MAX_NB_ARENAS 8
# define FT_MAL_MAX_THREADS 410669


/*
*************************** Global variables ********************************
*/

/*
*********** Pointer to arenas ****************
*/
t_s_ft_mal_state	*g_ft_arena = NULL;


static pid_t			gettid(void)
{
	pid_t tid = (pid_t)syscall(SYS_gettid);
	return tid;
}


static t_s_ft_mal_state	*ft_mal_new_arena(size_t alloc_size)
{
	t_s_ft_mal_state		*arena;
	t_s_ft_mal_heap_info	*heap_info;

	heap_info = ft_mal_new_heap(alloc_size, true);
	
	// error check
	if (!heap_info)
		return (NULL);
	
	arena = heap_info->ar_ptr;

	// initialize arena
	ft_bzero((void*)arena, sizeof(t_s_ft_mal_state));
	
	// check mutex initialization
	if (pthread_mutex_init(&arena->mutex, NULL) != FT_MAL_SUCCESS)
	{
		ft_mal_free_heap(heap_info);
		return (NULL);
	}
	arena->next = NULL;
	
	return (arena);
}

static bool				ft_mal_is_available_arena(t_s_ft_mal_state *arena)
{
	if (!arena)
		return (false);
	if (FT_MAL_MUTEX_TRYLOCK(&arena->mutex) == FT_MAL_SUCCESS)
		return (true);
	return (false);
}

static bool				ft_mal_arena_tid(size_t arena_id, pid_t tid, bool is_writing)
{
	static bool	arenas_tid[FT_MAL_MAX_NB_ARENAS][FT_MAL_MAX_THREADS];
	bool		res;

	if (is_writing)
		arenas_tid[arena_id][tid] = true;
	res = arenas_tid[arena_id][tid];
	return (res);
}

static t_s_ft_mal_state	*ft_mal_get_saved_arena_tid(t_s_ft_mal_state *arena, pid_t tid)
{
	while (arena)
	{
		if (ft_mal_arena_tid(arena->arena_id, tid, false))
		{
			FT_MAL_MUTEX_LOCK(&arena->mutex);
			return (arena);
		}
		arena = arena->next;
	}
	return (NULL);
}

static t_s_ft_mal_state	*ft_mal_find_available_arena(t_s_ft_mal_state **arena, size_t alloc_size)
{
	t_s_ft_mal_state	*res_arena;

	res_arena = *arena;
	while (true)
	{
		// check limit of arenas creation
		if (!res_arena && (!*arena || (*arena)->arena_id < FT_MAL_MAX_NB_ARENAS))
		{
			// create new arena
			res_arena = ft_mal_new_arena(alloc_size);

			// error check
			if (!res_arena)
				return (NULL);

			res_arena->next = *arena;
			*arena = res_arena;
			(*arena)->arena_id = ((*arena)->next ? (*arena)->next->arena_id + 1 : 0);
		}
		
		// continue loop from the beginning if all arenas are locked
		if (!res_arena)
			res_arena = *arena;
		else
		{
			if (ft_mal_is_available_arena(res_arena))
				break ;
			res_arena = res_arena->next;
		}
	}

	return (res_arena);
}

t_s_ft_mal_state		*ft_mal_get_available_arena(size_t alloc_size)
{
	pid_t					tid;
	t_s_ft_mal_state		*res_arena;

	// get current thread id
	tid = gettid();

	// check saved tids for current arenas
	res_arena = ft_mal_get_saved_arena_tid(g_ft_arena, tid);
	
	// find empty arena or create new, if there is no saved arena for current thread
	if (!res_arena)
	{
		res_arena = ft_mal_find_available_arena(&g_ft_arena, alloc_size);
		
		// save arena for current thread
		ft_mal_arena_tid(res_arena->arena_id, tid, true);
	}
	
	return (res_arena);
}

t_s_ft_mal_chunk		*ft_mal_get_available_chunk(t_s_ft_mal_state *arena, size_t alloc_size)
{

}
