/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_mal_arena.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aashara- <aashara-@student.21-school.ru    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 16:23:18 by aashara-          #+#    #+#             */
/*   Updated: 2021/10/31 22:45:17 by aashara-         ###   ########.fr       */
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
# define FT_MAL_MAX_NB_ARENAS 8		// number of arenas
# define FT_MAL_MAX_THREADS 410669	// max number of threads


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

// is arena used by another thread (lock mutex)
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
			return (arena);
		arena = arena->next;
	}
	return (NULL);
}

// add arena to start of the arenas list
static void				ft_mal_add_arena_to_list(t_s_ft_mal_state **head, t_s_ft_mal_state *arena)
{
	// assign next element
	arena->next = *head;
	
	// add element to the start of the list
	*head = arena;
}


// find arena that is not in use by other thread or create new arena (locking mutex)
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
			ft_mal_add_arena_to_list(arena, res_arena);

			// lock mutex
			FT_MAL_MUTEX_LOCK(&res_arena->mutex);

			break ;
		}
		
		// continue loop from the beginning if all arenas are locked
		if (!res_arena)
		{
			res_arena = *arena;
			continue ;
		}
		else if (*arena && (*arena)->arena_id >= FT_MAL_MAX_NB_ARENAS)
		{
			// locking mutex if return value is true
			if (ft_mal_is_available_arena(res_arena))
				break ;
		}
		res_arena = res_arena->next;

	}

	return (res_arena);
}

// checks is this pointer was allocated in this arena
static bool	ft_mal_is_arenas_ptr(t_s_ft_mal_state *arena, void *ptr)
{
	t_s_ft_mal_heap_info	*heap;
	t_e_ft_mal_heap_type	heap_type;


	// maybe arena is not created
	if (!arena)
		return (false);

	// determine heap type by ptr
	heap_type = ft_mal_get_heap_type_by_ptr(ptr);
	
	// lock mutex for iterating throw arena
	FT_MAL_MUTEX_LOCK(&arena->mutex);

	// search throw all heaps
	heap = arena->heaps;
	while (heap)
	{
		// is heap type equal
		if (heap_type == heap->heap_type)
		{
			// is pointer inside heap
			if (ptr > (void*)heap && (ptr - (void*)heap) <= (long int)heap->total_size)
				return (true);
		}
		heap = heap->next;
	}

	// unlock mutex, if arena is not found
	FT_MAL_MUTEX_UNLOCK(&arena->mutex);
	return (false);
}

// get arena by pointer (locking mutex)(search throw all arenas and heaps)(first search in current thread arena)
t_s_ft_mal_state		*ft_mal_get_arena_by_ptr(void *ptr)
{
	t_s_ft_mal_state	*current_tid_arena;
	t_s_ft_mal_state	*res_arena;
	pid_t				tid;

	// get current thread id
	tid = gettid();

	// find saved arena for current thread id
	current_tid_arena = ft_mal_get_saved_arena_tid(g_ft_arena, tid);

	// check is the memory allocated in the same thread (locking mutex)
	if (ft_mal_is_arenas_ptr(current_tid_arena, ptr))
		return (current_tid_arena);

	// search throw all arenas
	res_arena = g_ft_arena;
	while (res_arena)
	{
		if (res_arena != current_tid_arena
			&& ft_mal_is_arenas_ptr(res_arena, ptr)) //(locking mutex)
			return (res_arena);
		res_arena = res_arena->next;
	}
	return (NULL);
}

// get available arena for current thread (locking mutex)
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
		// locking mutex
		res_arena = ft_mal_find_available_arena(&g_ft_arena);
		
		// save arena for current thread id
		ft_mal_arena_tid(res_arena->arena_id, tid, true);
	}
	else
		FT_MAL_MUTEX_LOCK(&res_arena->mutex); // locking mutex
	
	return (res_arena);
}

// allocate memory from tiny chunks list
static void	*ft_mal_allocate_tiny_chunk(t_s_ft_mal_state *arena, size_t alloc_size)
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
	if (chunk && chunk->size >= alloc_size)
	{
		// assign alloc size
		chunk->size = alloc_size;
		
		ptr = FT_MAL_CHUNK_SHIFT(chunk);

		// remove chunk from list of empty tiny chunks
		ft_mal_remove_chunk_from_list(&arena->free_tiny_chunks, chunk);
	}
	
	return (ptr);
}

// get first fitting chunk from list of empty chunks and remove it from list
static t_s_ft_mal_chunk		*ft_mal_get_first_fit_small_chunk(t_s_ft_mal_chunk *chunks_list, size_t alloc_size)
{
	t_s_ft_mal_chunk	*current_chunk;
	t_s_ft_mal_chunk	*tmp_chunk;

	current_chunk = chunks_list;
	while (current_chunk)
	{
		if (current_chunk->size == alloc_size)
			break ;
		else if (current_chunk->size >= alloc_size +
			FT_MAL_SMALL_CHUNK_MIN_ALLOC_SIZE + FT_MAL_CHUNK_SIZE)	// we need size for next chunk
		{
			// remove current chunk from the list and create next element

			// next chunk initialization and adding it to list
			tmp_chunk = (void*)FT_MAL_CHUNK_SHIFT(current_chunk) + alloc_size;
			ft_bzero((void*)tmp_chunk, FT_MAL_CHUNK_SIZE);
			tmp_chunk->size = current_chunk->size - (alloc_size + FT_MAL_CHUNK_SIZE);
			tmp_chunk->prev = current_chunk;
			tmp_chunk->next = current_chunk->next;
			if (tmp_chunk->next)
				tmp_chunk->next->prev = tmp_chunk;
			
			// change current_chunk next element to remove it correctly
			current_chunk->size = alloc_size;
			current_chunk->next = tmp_chunk;

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

	chunk = ft_mal_get_first_fit_small_chunk(arena->free_small_chunks, alloc_size);

	// if there is no chunk enough for this memory, we should allocate new heap
	if (!chunk)
	{
		heap_info = ft_mal_new_heap(arena, FT_MAL_SMALL_HEAP_TYPE, 0);
	
		// error check
		if (!heap_info)
			return (NULL);

		chunk = ft_mal_get_first_fit_small_chunk(arena->free_small_chunks, alloc_size);
	}

	// assign pointer
	ptr = NULL;
	if (chunk && chunk->size == alloc_size)
	{
		ptr = FT_MAL_CHUNK_SHIFT(chunk);

		// remove chunk from list of empty small chunks
		ft_mal_remove_chunk_from_list(&arena->free_small_chunks, chunk);
	}
	
	return (ptr);
}

// allocate memory from large chunks list
static void	*ft_mal_allocate_large_chunk(t_s_ft_mal_state *arena, size_t alloc_size)
{
	t_s_ft_mal_heap_info	*heap_info;
	t_s_ft_mal_chunk		*chunk;
	void					*ptr;

	// we should allocate new heap
	heap_info = ft_mal_new_heap(arena, FT_MAL_LARGE_HEAP_TYPE, alloc_size);
	
	// error check
	if (!heap_info)
		return (NULL);

	// assign pointer
	ptr = NULL;
	chunk = arena->free_large_chunks;
	if (chunk && chunk->size >= alloc_size)
	{
		// assign alloc size
		chunk->size = alloc_size;
		
		ptr = FT_MAL_CHUNK_SHIFT(chunk);

		// remove chunk from list of empty tiny chunks
		ft_mal_remove_chunk_from_list(&arena->free_large_chunks, chunk);
	}
	
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
		ptr = ft_mal_allocate_tiny_chunk(arena, alloc_size);
	else if (heap_type == FT_MAL_SMALL_HEAP_TYPE)
	{
		// merge small continious blocks in one
		ft_mal_merge_chunks(arena->free_small_chunks);
		ptr = ft_mal_allocate_small_chunk(arena, alloc_size);
	}
	else if (heap_type == FT_MAL_LARGE_HEAP_TYPE)
		ptr = ft_mal_allocate_large_chunk(arena, alloc_size);

	return ptr;
}

// free tiny chunks of memory
static void	ft_mal_free_tiny_chunk(t_s_ft_mal_state *arena, void *ptr)
{
	t_s_ft_mal_chunk	*chunk;

	chunk = FT_MAL_CHUNK_BACK_SHIFT(ptr);

	// assign max size of tiny chunks
	chunk->size = FT_MAL_TINY_CHUNK_MAX_ALLOC_SIZE;

	// add freed tiny chunk to list
	ft_mal_add_chunk_to_list(&arena->free_tiny_chunks, chunk);
}

// free small chunks of memory
static void	ft_mal_free_small_chunk(t_s_ft_mal_state *arena, void *ptr)
{
	t_s_ft_mal_chunk	*chunk;
	
	chunk = FT_MAL_CHUNK_BACK_SHIFT(ptr);
	
	// add freed small chunk to list
	ft_mal_add_chunk_to_list(&arena->free_small_chunks, chunk);
	
	// sort list by adress
	ft_mal_sort_chunks(&arena->free_small_chunks);
}

// free large chunks of memory
static void	ft_mal_free_large_chunk(t_s_ft_mal_state *arena, void *ptr)
{
	t_s_ft_mal_heap_info	*heap_info;

	heap_info = FT_MAL_HEAP_INFO_BACK_SHIFT(FT_MAL_CHUNK_BACK_SHIFT(ptr));
	// remove full heap
	ft_mal_free_heap(&arena->heaps, heap_info);
}

// free memory from arena
void	ft_mal_free_memory(t_s_ft_mal_state *arena, void *ptr)
{
	t_e_ft_mal_heap_type	heap_type;
	t_s_ft_mal_chunk		*chunk;

	// determine heap type by ptr
	heap_type = ft_mal_get_heap_type_by_ptr(ptr);

	// get chunk
	chunk = FT_MAL_CHUNK_BACK_SHIFT(ptr);
	
	//already freed pointer
	if (chunk->prev || chunk->next)
		return ;

	if (heap_type == FT_MAL_TINY_HEAP_TYPE)
		ft_mal_free_tiny_chunk(arena, ptr);
	else if (heap_type == FT_MAL_SMALL_HEAP_TYPE)
		ft_mal_free_small_chunk(arena, ptr);
	else if (heap_type == FT_MAL_LARGE_HEAP_TYPE)
		ft_mal_free_large_chunk(arena, ptr);
}

// realloc tiny chunks (if we can use the same chunk, we don't need to realloc memory)
static void	*ft_mal_realloc_tiny_chunk(t_s_ft_mal_state *arena, void *ptr, size_t size)
{
	t_e_ft_mal_heap_type	new_heap_type;
	t_s_ft_mal_chunk		*chunk;
	void					*new_ptr;
	
	// get chunk
	chunk = FT_MAL_CHUNK_BACK_SHIFT(ptr);
	
	// determine new heap type by size
	new_heap_type = ft_mal_get_heap_type_by_alloc_size(size);
	if (new_heap_type == FT_MAL_TINY_HEAP_TYPE)
	{
		// chunk type is the same and there is space for new size
		// assign new_size
		chunk->size = size;
	}
	else
	{
		new_ptr = ft_mal_allocate_memory(arena, size);

		// error happened
		if (!new_ptr)
			return (NULL);

		// copy data from previous chunk
		ft_memcpy(new_ptr, ptr, size > chunk->size ? chunk->size : size);
		
		// free previous chunk
		ft_mal_free_memory(arena, ptr);
		
		ptr = new_ptr;
	}
	return (ptr);
}

// realloc small chunks
static void	*ft_mal_realloc_small_chunk(t_s_ft_mal_state *arena, void *ptr, size_t size)
{
	t_s_ft_mal_chunk		*chunk;
	void					*new_ptr;
	
	// get chunk
	chunk = FT_MAL_CHUNK_BACK_SHIFT(ptr);
	
	new_ptr = ft_mal_allocate_memory(arena, size);

	// error happened
	if (!new_ptr)
		return (NULL);

	// copy data from previous chunk
	ft_memcpy(new_ptr, ptr, size > chunk->size ? chunk->size : size);
	
	// free previous chunk
	ft_mal_free_memory(arena, ptr);
	
	return (new_ptr);
}

// realloc large chunks (if we can use the same chunk, we don't need to realloc memory)
static void	*ft_mal_realloc_large_chunk(t_s_ft_mal_state *arena, void *ptr, size_t size)
{
	t_e_ft_mal_heap_type	new_heap_type;
	t_s_ft_mal_chunk		*chunk;
	void					*new_ptr;
	t_s_ft_mal_heap_info	*heap;
	
	// get chunk
	chunk = FT_MAL_CHUNK_BACK_SHIFT(ptr);
	
	// determine new heap type by size
	new_heap_type = ft_mal_get_heap_type_by_alloc_size(size);

	if (new_heap_type == FT_MAL_LARGE_HEAP_TYPE)
	{
		// chunk type is the same and there is space for new size
		heap = FT_MAL_HEAP_INFO_BACK_SHIFT(chunk);
		
		if (heap->total_size - (ptr - (void*)heap) >= size)
		{
			// assign new_size
			chunk->size = size;
			return (ptr);
		}
	}
	
	new_ptr = ft_mal_allocate_memory(arena, size);

	// error happened
	if (!new_ptr)
		return (NULL);

	// copy data from previous chunk
	ft_memcpy(new_ptr, ptr, size > chunk->size ? chunk->size : size);
		
	// free previous chunk
	ft_mal_free_memory(arena, ptr);
		
	return (new_ptr);
}

// realloc memory with arena
void	*ft_mal_realloc_memory(t_s_ft_mal_state *arena, void *ptr, size_t size)
{
	t_e_ft_mal_heap_type	heap_type;

	// determine heap type by ptr
	heap_type = ft_mal_get_heap_type_by_ptr(ptr);

	if (heap_type == FT_MAL_TINY_HEAP_TYPE)
		return (ft_mal_realloc_tiny_chunk(arena, ptr, size));
	else if (heap_type == FT_MAL_SMALL_HEAP_TYPE)
		return (ft_mal_realloc_small_chunk(arena, ptr, size));
	else if (heap_type == FT_MAL_LARGE_HEAP_TYPE)
		return (ft_mal_realloc_large_chunk(arena, ptr, size));
	return (NULL);
}

// return arenas global variable head (get list of all arenas)
t_s_ft_mal_state			*ft_mal_get_arenas_list(void)
{
	return (g_ft_arena);
}
