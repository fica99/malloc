/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_mal_arena.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aashara- <aashara-@student.21-school.ru    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 16:23:18 by aashara-          #+#    #+#             */
/*   Updated: 2021/10/30 16:10:00 by aashara-         ###   ########.fr       */
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


// get thread id
static pid_t			gettid(void)
{
	pid_t tid = (pid_t)syscall(SYS_gettid);
	return tid;
}

// create new arena
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
	
	// initialize mutex for this arena
	if (pthread_mutex_init(&arena->mutex, NULL) != FT_MAL_SUCCESS)
	{
		// error happened on mutex initialization
		ft_mal_free_heap(&arena->heaps, heap_info);
		return (NULL);
	}
	
	return (arena);
}

// is arena used by another thread
static bool				ft_mal_is_available_arena(t_s_ft_mal_state *arena)
{
	if (!arena)
		return (false);
	if (FT_MAL_MUTEX_TRYLOCK(&arena->mutex) == FT_MAL_SUCCESS)
		return (true);
	return (false);
}

// check or save arena id for thread id
static bool				ft_mal_arena_tid(size_t arena_id, pid_t tid, bool is_writing)
{
	static bool	arenas_tid[FT_MAL_MAX_NB_ARENAS][FT_MAL_MAX_THREADS];
	bool		res;

	if (is_writing)
		arenas_tid[arena_id % FT_MAL_MAX_NB_ARENAS][tid % FT_MAL_MAX_THREADS] = true;
	res = arenas_tid[arena_id % FT_MAL_MAX_NB_ARENAS][tid % FT_MAL_MAX_THREADS];
	return (res);
}

// get saved arena for current thread id
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

// find arena that is not in use by other thread or create new arena
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

// get saved arena for current thread
t_s_ft_mal_state		*ft_mal_get_saved_arena(void)
{
	pid_t				tid;
	t_s_ft_mal_state	*res_arena;

	// get current thread id
	tid = gettid();

	// find saved arena for current thread id
	res_arena = ft_mal_get_saved_arena_tid(g_ft_arena, tid);
	
	return (res_arena);
}

// get available arena for current thread
t_s_ft_mal_state		*ft_mal_get_available_arena(void)
{
	pid_t				tid;
	t_s_ft_mal_state	*res_arena;

	// get current thread id
	tid = gettid();

	// find saved arena for current thread id
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

// allocate memory from tiny chunks list
static void	*ft_mal_allocate_tiny_chunk(t_s_ft_mal_state *arena)
{
	t_s_ft_mal_heap_info	*heap_info;
	t_s_ft_mal_chunk		*chunk;
	void					*ptr;

	// if there is no tiny chunk, we should allocate new heap
	if (!arena->free_tiny_chunks)
	{
		heap_info = ft_mal_new_heap(arena, FT_MAL_TINY_HEAP_TYPE, 0);
	
		// error check
		if (!heap_info)
			return (NULL);
	}

	// assign pointer
	ptr = NULL;
	chunk = arena->free_tiny_chunks;
	if (chunk && chunk->size >= FT_MAL_TINY_CHUNK_MAX_ALLOC_SIZE)
		ptr = FT_MAL_CHUNK_SHIFT(chunk);

	// remove chunk from list of empty tiny chunks
	if (ptr)
		ft_mal_remove_chunk_from_list(&arena->free_tiny_chunks, chunk);
	
	return (ptr);
}

// get first fitting chunk from list of empty chunks and remove it from list
static t_s_ft_mal_chunk		*ft_mal_get_first_fit_small_chunk(t_s_ft_mal_chunk **chunks_list, size_t alloc_size)
{
	t_s_ft_mal_chunk	*current_chunk;
	t_s_ft_mal_chunk	*tmp_chunk;

	current_chunk = *chunks_list;
	while (current_chunk)
	{
		if (current_chunk->size == alloc_size)
		{
			// remove current chunk from the list
			ft_mal_remove_chunk_from_list(chunks_list, current_chunk);
			break ;
		}
		else if (current_chunk->size >= alloc_size +
			(FT_MAL_SMALL_CHUNK_MIN_ALLOC_SIZE + sizeof(t_s_ft_mal_chunk)))	// we need size for next chunk
		{
			// remove current chunk from the list and create next element

			// next chunk initialization and adding it to list
			tmp_chunk = (void*)FT_MAL_CHUNK_SHIFT(current_chunk) + alloc_size;
			ft_bzero((void*)tmp_chunk, sizeof(t_s_ft_mal_chunk));
			tmp_chunk->size = current_chunk->size - (alloc_size + sizeof(t_s_ft_mal_chunk));
			tmp_chunk->prev = current_chunk;
			tmp_chunk->next = current_chunk->next;
			if (tmp_chunk->next)
				tmp_chunk->next->prev = tmp_chunk;
			
			// change current_chunk next element to remove it correctly
			current_chunk->size = alloc_size;
			current_chunk->next = tmp_chunk;

			// remove current chunk from the list
			ft_mal_remove_chunk_from_list(chunks_list, current_chunk);
			break ;
		}
		current_chunk = current_chunk->next;
	}

	return (current_chunk);
}

// allocate memory from small chunks list
static void	*ft_mal_allocate_small_chunk(t_s_ft_mal_state *arena, size_t alloc_size)
{
	t_s_ft_mal_heap_info	*heap_info;
	t_s_ft_mal_chunk		*chunk;
	void					*ptr;

	chunk = ft_mal_get_first_fit_small_chunk(&arena->free_small_chunks, alloc_size);

	// if there is no chunk enough for this memory, we should allocate new heap
	if (!chunk)
	{
		heap_info = ft_mal_new_heap(arena, FT_MAL_SMALL_HEAP_TYPE, 0);
	
		// error check
		if (!heap_info)
			return (NULL);

		chunk = ft_mal_get_first_fit_small_chunk(&arena->free_small_chunks, alloc_size);
	}

	// assign pointer
	ptr = NULL;
	if (chunk && chunk->size >= alloc_size)
		ptr = FT_MAL_CHUNK_SHIFT(chunk);
	
	return (ptr);
}

// merge small continious blocks in one
static void	ft_mal_merge_small_chunks(t_s_ft_mal_chunk *free_small_chunks)
{
	t_s_ft_mal_chunk	*current;
	t_s_ft_mal_chunk	*next;

	current = free_small_chunks;
	while (current)
	{
		// next element
		next = current->next;
		if (next)
		{
			// if blocks are connected, we can merge it
			if (FT_MAL_CHUNK_SHIFT(current) + current->size == (void*)next)
			{
				// merge size and header
				current->size += (next->size + sizeof(t_s_ft_mal_chunk));
				
				// remove merged element from list
				// we sending pointer to head, but head element cannot be removed
				ft_mal_remove_chunk_from_list(&free_small_chunks, next);
				
				continue;
			}
		}
		current = next;
	}
}

// allocate memory from large chunks list
static void	*ft_mal_allocate_large_chunk(t_s_ft_mal_state *arena, size_t alloc_size)
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
	}

	// assign pointer
	ptr = NULL;
	chunk = arena->free_large_chunks;
	if (chunk && chunk->size >= alloc_size)
		ptr = FT_MAL_CHUNK_SHIFT(chunk);

	// remove chunk from list of empty tiny chunks
	if (ptr)
		ft_mal_remove_chunk_from_list(&arena->free_large_chunks, chunk);
	
	return (ptr);
}

// allocate memory of alloc size from arena
void		*ft_mal_allocate_memory(t_s_ft_mal_state *arena, size_t alloc_size)
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
		ptr = ft_mal_allocate_tiny_chunk(arena);
	else if (heap_type == FT_MAL_SMALL_HEAP_TYPE)
	{
		// merge small continious blocks in one
		ft_mal_merge_small_chunks(arena->free_small_chunks);
		ptr = ft_mal_allocate_small_chunk(arena, alloc_size);
	}
	else if (heap_type == FT_MAL_LARGE_HEAP_TYPE)
		ptr = ft_mal_allocate_large_chunk(arena, alloc_size);

	return ptr;
}

// free memory from arena
void	ft_mal_free_memory(t_s_ft_mal_state *arena, void *ptr)
{
	// t_e_ft_mal_heap_type	heap_type;

	// // determine heap group by ptr
	// heap_type = ft_mal_get_heap_type_by_ptr(ptr);
	
}
