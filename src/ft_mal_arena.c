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
# define FT_MAL_MAX_NB_ARENAS 8 // should change it
# define FT_MAL_MAX_THREADS 410669 // max number of threads


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


static void				ft_mal_add_new_heap(t_s_ft_mal_state *arena, t_s_ft_mal_heap_info *heap_info)
{
	// add heap to heaps list

	// assign next element
	heap_info->next = arena->heaps;
	
	// assign previous element
	if (arena->heaps)
		arena->heaps->prev = heap_info;
	
	// add element to the start of the list
	arena->heaps = heap_info;
}

static t_s_ft_mal_state	*ft_mal_new_arena(void)
{
	t_s_ft_mal_state		*arena;
	t_s_ft_mal_heap_info	*heap_info;

	// initialize new heap
	heap_info = ft_mal_new_heap(NULL, FT_MAL_TINY_HEAP_TYPE, 0);
	
	// error check
	if (!heap_info)
		return (NULL);
	
	arena = heap_info->ar_ptr;

	ft_mal_add_new_heap(arena, heap_info);
	
	// check mutex initialization
	if (pthread_mutex_init(&arena->mutex, NULL) != FT_MAL_SUCCESS)
	{
		ft_mal_free_heap(heap_info);
		return (NULL);
	}
	
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
		arenas_tid[arena_id % FT_MAL_MAX_NB_ARENAS][tid % FT_MAL_MAX_THREADS] = true;
	res = arenas_tid[arena_id % FT_MAL_MAX_NB_ARENAS][tid % FT_MAL_MAX_THREADS];
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

static t_s_ft_mal_state	*ft_mal_find_available_arena(t_s_ft_mal_state **arena)
{
	t_s_ft_mal_state	*res_arena;

	res_arena = *arena;
	while (true)
	{
		// create new arena if limit is not reached and there is no available arena
		if (!res_arena && (!*arena || (*arena)->arena_id < FT_MAL_MAX_NB_ARENAS))
		{
			// create new arena
			res_arena = ft_mal_new_arena();

			// error check
			if (!res_arena)
				return (NULL);

			// assign id to new arena
			res_arena->arena_id = ((*arena) ? (*arena)->arena_id + 1 : 0);
			
			// add arena to the start of the list
			res_arena->next = *arena;
			*arena = res_arena;
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

t_s_ft_mal_state		*ft_mal_get_available_arena(void)
{
	pid_t					tid;
	t_s_ft_mal_state		*res_arena;

	// get current thread id
	tid = gettid();

	// check saved arenas for current thread id
	res_arena = ft_mal_get_saved_arena_tid(g_ft_arena, tid);
	
	// find empty arena or create new, if there is no saved arena for current thread
	if (!res_arena)
	{
		res_arena = ft_mal_find_available_arena(&g_ft_arena);
		
		// save arena for current thread id
		ft_mal_arena_tid(res_arena->arena_id, tid, true);
	}
	
	return (res_arena);
}

static void	*ft_mal_allocate_memory_from_arena_tiny(t_s_ft_mal_state *arena)
{
	t_s_ft_mal_heap_info	*heap_info;
	void					*ptr;

	// if there is no tiny chunk, we should allocate new heap
	if (!arena->free_tiny_chunks)
	{
		heap_info = ft_mal_new_heap(arena, FT_MAL_TINY_HEAP_TYPE, 0);
	
		// error check
		if (!heap_info)
			return (NULL);
	
		ft_mal_add_new_heap(arena, heap_info);
	}

	// assign pointer
	ptr = arena->free_tiny_chunks;

	// remove chunk from list of empty tiny chunks
	if (ptr)
		arena->free_tiny_chunks = arena->free_tiny_chunks->next;
	
	return (ptr);
}

static void	*ft_mal_allocate_memory_from_arena_large(t_s_ft_mal_state *arena, size_t alloc_size)
{
	t_s_ft_mal_heap_info	*heap_info;
	t_s_ft_mal_chunk		*chunk;
	void					*ptr;

	// if there is no large chunk, we should allocate new heap
	if (!arena->free_large_chunks)
	{
		heap_info = ft_mal_new_heap(arena, FT_MAL_LARGE_HEAP_TYPE, alloc_size);
	
		// error check
		if (!heap_info)
			return (NULL);
	
		ft_mal_add_new_heap(arena, heap_info);
	}

	// assign pointer
	ptr = NULL;
	chunk = arena->free_large_chunks;
	if (chunk && chunk->size >= alloc_size)
		ptr = FT_MAL_CHUNK_SHIFT(chunk);

	// remove chunk from list of empty tiny chunks
	if (ptr)
		arena->free_large_chunks = arena->free_large_chunks->next;
	
	return (ptr);
}

void		*ft_mal_allocate_memory_from_arena(t_s_ft_mal_state *arena, size_t alloc_size)
{
	t_e_ft_mal_heap_type	heap_type;
	void					*ptr;

	// return NULL if it was requested 0 bytes of memory
	if (!alloc_size)
		return (NULL);

	// determine heap group by allocation size
	heap_type = ft_mal_get_heap_type_by_alloc_size(alloc_size);

	ptr = NULL;
	if (heap_type == FT_MAL_TINY_HEAP_TYPE)
	{
		ptr = ft_mal_allocate_memory_from_arena_tiny(arena);
	} else if (heap_type == FT_MAL_SMALL_HEAP_TYPE)
	{
		// merge small blocks
		ptr = NULL; // fix it
	}
	else if (heap_type == FT_MAL_LARGE_HEAP_TYPE)
	{
		ptr = ft_mal_allocate_memory_from_arena_large(arena, alloc_size);
	}

	return ptr;
}
