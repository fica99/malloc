/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_mal_heap.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aashara- <aashara-@student.21-school.ru    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 16:23:18 by aashara-          #+#    #+#             */
/*   Updated: 2021/10/30 12:09:29 by aashara-         ###   ########.fr       */
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
*********** Heap header *********************
*/
#include "ft_mal_heap.h"

/*
*********** Chunk struct ********************
*********** Chunk type **********************
*/
#include "ft_mal_chunk.h"


/*
*************************** System headers **********************************
*/

/*
*********** getpagesize *************************
*/
# include <unistd.h>

/*
*********** mmap ********************************
*********** munmap ******************************
*/
#include <sys/mman.h>

/*
*********** getrlimit ***************************
*/
#include <sys/time.h>
#include <sys/resource.h>


/*
*************************** Macroses ****************************************
*/

/*
*********** Number of chunks in heap ************
*/
# define FT_MAL_NB_OF_TINY_CHUNKS 240
# define FT_MAL_NB_OF_SMALL_CHUNKS 120

static size_t			ft_mal_get_heap_total_size(t_s_ft_mal_state *arena,
							t_e_ft_mal_heap_type heap_type, size_t large_chunk_size)
{
	size_t					total_size;
	size_t					page_size;
	size_t					nb_pages;

	// determine pagesize
	page_size = getpagesize();

	// heap metadata
	total_size = sizeof(t_s_ft_mal_heap_info);

	// arena metadata
	if (!arena)
		total_size += sizeof(t_s_ft_mal_state);
	
	if (heap_type == FT_MAL_TINY_HEAP_TYPE)
		total_size += ((FT_MAL_TINY_CHUNK_MAX_ALLOC_SIZE + sizeof(t_s_ft_mal_chunk)) * FT_MAL_NB_OF_TINY_CHUNKS);
	else if (heap_type == FT_MAL_SMALL_HEAP_TYPE)
		total_size += ((FT_MAL_SMALL_CHUNK_MAX_ALLOC_SIZE + sizeof(t_s_ft_mal_chunk)) * FT_MAL_NB_OF_SMALL_CHUNKS);
	else if (heap_type == FT_MAL_LARGE_HEAP_TYPE)
		total_size += (large_chunk_size + sizeof(t_s_ft_mal_chunk));

	// determine number of pages
	nb_pages = total_size / page_size;
	if (total_size % page_size)
		++nb_pages;

	return (nb_pages * page_size);
}

static rlim_t			ft_mal_get_memory_limit(void)
{
	struct rlimit rpl;

	if (getrlimit(RLIMIT_AS, &rpl) < 0)
		return (-1);
	return (rpl.rlim_max);
}

static void				ft_mal_add_new_empty_chunks(t_s_ft_mal_state *arena, t_s_ft_mal_heap_info *heap_info)
{
	void				*start;
	size_t				i;
	t_s_ft_mal_chunk	*current_chunk;
	
	// determine start of the memory 
	start = FT_MAL_HEAP_INFO_SHIFT(heap_info);
	if (start == arena)
		start = FT_MAL_STATE_SHIFT(start);
	
	// initialize header
	ft_bzero(start, sizeof(t_s_ft_mal_chunk));

	current_chunk = start;

	if (heap_info->heap_type == FT_MAL_TINY_HEAP_TYPE)
	{
		// add chunks from block to tiny chunks list
		i = 0;
		while (i + (sizeof(t_s_ft_mal_chunk)
			+ FT_MAL_TINY_CHUNK_MAX_ALLOC_SIZE) < heap_info->total_size)
		{
			// initialize header
			ft_bzero(start + i, sizeof(t_s_ft_mal_chunk));

			current_chunk = start + i;

			current_chunk->size = FT_MAL_TINY_CHUNK_MAX_ALLOC_SIZE;

			// add chunk to start of tiny chunks list

			// assign next element
			current_chunk->next = arena->free_tiny_chunks;
	
			// assign previous element
			if (arena->free_tiny_chunks)
				arena->free_tiny_chunks->prev = current_chunk;
				
			// add element to the start of the list
			arena->free_tiny_chunks = current_chunk;
		
			i += (FT_MAL_TINY_CHUNK_MAX_ALLOC_SIZE + sizeof(t_s_ft_mal_chunk));
		}
	}
	else if (heap_info->heap_type == FT_MAL_SMALL_HEAP_TYPE)
	{
		// add one big chunk(size of block) to start of the list
		
		// assign size of the chunk
		current_chunk->size = heap_info->total_size - (FT_MAL_CHUNK_SHIFT(start) - (void*)heap_info);

		// assign next element
		current_chunk->next = arena->free_small_chunks;
	
		// assign previous element
		if (arena->free_small_chunks)
			arena->free_small_chunks->prev = current_chunk;

		// add element to the start of the list
		arena->free_small_chunks = current_chunk;
	}
	else
	{
		// add one big chunk to start of the list
		
		// assign size of the chunk
		current_chunk->size = heap_info->total_size - (FT_MAL_CHUNK_SHIFT(start) - (void*)heap_info);

		// assign next element
		current_chunk->next = arena->free_large_chunks;
	
		// assign previous element
		if (arena->free_large_chunks)
			arena->free_large_chunks->prev = current_chunk;

		// add element to the start of the list
		arena->free_large_chunks = current_chunk;
	}
}

// add new heap to start of the heaps list
static void				ft_mal_add_new_heap(t_s_ft_mal_state *arena, t_s_ft_mal_heap_info *heap_info)
{
	// assign next element
	heap_info->next = arena->heaps;
	
	// assign previous element
	if (arena->heaps)
		arena->heaps->prev = heap_info;
	
	// add element to the start of the list
	arena->heaps = heap_info;
}

// allocate new different types of heap (with or without arena)
t_s_ft_mal_heap_info	*ft_mal_new_heap(t_s_ft_mal_state *arena, t_e_ft_mal_heap_type heap_type, size_t large_chunk_size)
{
	t_s_ft_mal_heap_info	*heap_info;
	size_t					total_size;
	static size_t			allocated_memory_size;
	
	// determine heap size to allocate
	total_size = ft_mal_get_heap_total_size(arena, heap_type, large_chunk_size);
	
	// check enough memory
	if (allocated_memory_size + total_size> ft_mal_get_memory_limit())
		return (NULL);

	// allocate heap
	heap_info = (t_s_ft_mal_heap_info *)mmap(NULL, total_size,
		PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);

	// error check
	if (heap_info == MAP_FAILED)
		return (NULL);

	// add allocated memory
	allocated_memory_size += total_size;

	// initialize heap info
	ft_bzero((void*)heap_info, sizeof(t_s_ft_mal_heap_info));
	heap_info->total_size = total_size;
	heap_info->heap_type = heap_type;

	// if not exist, initialize it
	if (!arena)
	{
		arena = FT_MAL_HEAP_INFO_SHIFT(heap_info);
		ft_bzero(arena, sizeof(t_s_ft_mal_state));
	}
	
	heap_info->ar_ptr = arena;

	// add allocated heap to heap list
	ft_mal_add_new_heap(arena, heap_info);

	// add new free chunks to lists
	ft_mal_add_new_empty_chunks(heap_info->ar_ptr, heap_info);

	return (heap_info);
}

// remove heap from list and free it
int					ft_mal_free_heap(t_s_ft_mal_heap_info **head, t_s_ft_mal_heap_info *heap)
{
	t_s_ft_mal_heap_info	*next;
	t_s_ft_mal_heap_info	*prev;

	if (heap)
	{
		// remove heap from the list

		next = heap->next;
		prev = heap->prev;

		// update next element
		if (next)
			next->prev = prev;

		// update previous element
		if (prev)
			prev->next = next;

		// if heap is head, remove it
		if (*head == heap)
			*head = next;

		// free heap total size
		return (munmap((void*)heap, heap->total_size)); // return function status
	}
	return (FT_MAL_SUCCESS);
}

t_e_ft_mal_heap_type	ft_mal_get_heap_type_by_alloc_size(size_t alloc_size)
{
	if (alloc_size >= FT_MAL_TINY_CHUNK_MIN_ALLOC_SIZE && alloc_size <= FT_MAL_TINY_CHUNK_MAX_ALLOC_SIZE)
		return (FT_MAL_TINY_HEAP_TYPE);
	else if (alloc_size >= FT_MAL_SMALL_CHUNK_MIN_ALLOC_SIZE && alloc_size <= FT_MAL_SMALL_CHUNK_MAX_ALLOC_SIZE)
		return (FT_MAL_SMALL_HEAP_TYPE);
	return (FT_MAL_LARGE_HEAP_TYPE);
}
