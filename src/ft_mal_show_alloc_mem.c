/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_mal_show_alloc_mem.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aashara- <aashara-@student.21-school.ru    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 15:58:58 by aashara-          #+#    #+#             */
/*   Updated: 2021/11/05 13:50:50 by aashara-         ###   ########.fr       */
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
*********** Print functions header **********
*/
#include "ft_mal_print.h"

/*
*********** Arena header ********************
*/
#include "ft_mal_arena.h"

/*
*********** Heaps header ********************
*/
#include "ft_mal_heap.h"

/*
*********** Chunk header ********************
*/
#include "ft_mal_chunk.h"


/*
************************** System headers ***********************************
*/

/*
************ PTHREAD_MUTEX_INITIALIZER *****
************ pthread_mutex_t ***************
*/
# include <pthread.h>


/*
*************************** Global variables ********************************
*/

/*
*********** writing mutex ********************
*/
static pthread_mutex_t	g_ft_mal_writing_mutex = PTHREAD_MUTEX_INITIALIZER;


static void		ft_mal_print_chunk_info(t_s_ft_mal_chunk *chunk)
{
	// print start of the chunk address
	ft_mal_print(NULL, (size_t)chunk, 16, NULL);

	// print end of the chunk address
	ft_mal_print(" - ", (size_t)(chunk + chunk->size), 16, " : ");

	// print size of the chunk
	ft_mal_print(NULL, chunk->size, 10, " bytes\n");
}

// show tiny heap
static size_t	ft_mal_show_tiny_heap(t_s_ft_mal_state *arena, t_s_ft_mal_heap_info *heap)
{
	t_s_ft_mal_chunk		*chunk;
	size_t					i;
	size_t					total_size;
	
	// print heap header address
	ft_mal_print("TINY : ", (size_t)heap, 16, "\n");
	
	// shift heap
	chunk = FT_MAL_HEAP_INFO_SHIFT(heap);
		
	// shift if there is arena on this heap
	if ((void*)chunk == (void*)arena)
		chunk = FT_MAL_STATE_SHIFT(chunk);
	
	// determine start number byte
	i = (void*)chunk - (void*)heap;

	total_size = 0;

	while (i < heap->total_size)
	{
		chunk = (void*)heap + i;
		// Check that element isn't in the list
		if (!chunk->next && !chunk->prev && chunk != arena->free_tiny_chunks && chunk->size)
		{
			// print chunk info
			ft_mal_print_chunk_info(chunk);

			total_size += chunk->size;
		}

		// next chunk
		i += FT_MAL_TINY_CHUNK_SIZE;
	}

	return (total_size);
}

// show small heap
static size_t	ft_mal_show_small_heap(t_s_ft_mal_state *arena, t_s_ft_mal_heap_info *heap)
{
	t_s_ft_mal_chunk		*chunk;
	size_t					i;
	size_t					total_size;
	
	// print heap header address
	ft_mal_print("SMALL : ", (size_t)heap, 16, "\n");

	// shift heap
	chunk = FT_MAL_HEAP_INFO_SHIFT(heap);
	
	// determine start number byte
	i = (void*)chunk - (void*)heap;

	total_size = 0;

	while (i < heap->total_size)
	{
		// Check that element isn't in the list
		if (!chunk->next && !chunk->prev && arena->free_small_chunks != chunk)
		{
			// print chunk info
			ft_mal_print_chunk_info(chunk);
			
			total_size += chunk->size;
		}

		// next chunk
		chunk = (void*)FT_MAL_CHUNK_SHIFT(chunk) + chunk->size;
		i = (void*)chunk - (void*)heap;
	}

	return (total_size);
}

// show large heap
static size_t	ft_mal_show_large_heap(t_s_ft_mal_state *arena, t_s_ft_mal_heap_info *heap)
{
	t_s_ft_mal_chunk		*chunk;
	
	// print heap header address
	ft_mal_print("LARGE : ", (size_t)heap, 16, "\n");
	
	// shift heap
	chunk = FT_MAL_HEAP_INFO_SHIFT(heap);

	// Check that element isn't in the list
	if (!chunk->prev && !chunk->next && arena->free_large_chunks != chunk)
	{
		// print chunk info
		ft_mal_print_chunk_info(chunk);
		
		return (chunk->size);
	}
	return (0);
}

// print heaps
static void	ft_mal_show_heaps(t_s_ft_mal_state *arena)
{
	t_s_ft_mal_heap_info	*heap;
	size_t					total_allocated_size;

	heap = arena->heaps;
	total_allocated_size = 0;
	while (heap)
	{
		if (heap->heap_type == FT_MAL_TINY_HEAP_TYPE)
			total_allocated_size += ft_mal_show_tiny_heap(arena, heap);
		else if (heap->heap_type == FT_MAL_SMALL_HEAP_TYPE)
			total_allocated_size += ft_mal_show_small_heap(arena, heap);
		else if (heap->heap_type == FT_MAL_LARGE_HEAP_TYPE)
			total_allocated_size += ft_mal_show_large_heap(arena, heap);

		heap = heap->next;
	}
	ft_mal_print("Total : ", total_allocated_size, 10, " bytes\n");
}

void		show_alloc_mem(void)
{
	t_s_ft_mal_state	*arena;

	// get head of arenas list
	arena = ft_mal_get_arenas_list();
	
	while (arena)
	{
		// lock mutex for arena
		FT_MAL_MUTEX_LOCK(&arena->mutex);

		// sort heaps
		ft_mal_sort_heaps(&arena->heaps);

		// lock mutex for writing
		FT_MAL_MUTEX_LOCK(&g_ft_mal_writing_mutex);

		// print arena header
		ft_mal_print("Arena ID : ", arena->arena_id, 10, "\n");

		// print heaps
		ft_mal_show_heaps(arena);

		// unlock mutex for writing
		FT_MAL_MUTEX_UNLOCK(&g_ft_mal_writing_mutex);

		// unlock mutex for arena
		FT_MAL_MUTEX_UNLOCK(&arena->mutex);
	
		arena = arena->next;
	}	
}
