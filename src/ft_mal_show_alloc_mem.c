/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_mal_show_alloc_mem.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aashara- <aashara-@student.21-school.ru    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 15:58:58 by aashara-          #+#    #+#             */
/*   Updated: 2021/11/04 13:36:44 by aashara-         ###   ########.fr       */
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

/*
*********** Heaps header ********************
*/
#include "ft_mal_heap.h"

/*
*********** ft_printf ********************
*/
#include "ft_printf.h"


// show tiny heap
static size_t	ft_mal_show_tiny_heap(t_s_ft_mal_state *arena, t_s_ft_mal_heap_info *heap)
{
	t_s_ft_mal_chunk		*chunk;
	size_t					i;
	size_t					total_size;
	
	// printf start of the heap
	ft_printf("TINY : %p\n", (void*)heap);

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
			ft_printf("%p - %p : %zu bytes\n", (void*)chunk, (void*)chunk + chunk->size, chunk->size);
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
	
	// printf start of the heap
	ft_printf("SMALL : %p\n", (void*)heap);

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
			ft_printf("%p - %p : %zu bytes\n", (void*)chunk, (void*)chunk + chunk->size, chunk->size);
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
	
	// printf start of the heap
	ft_printf("LARGE : %p\n", (void*)heap);
	
	// shift heap
	chunk = FT_MAL_HEAP_INFO_SHIFT(heap);

	// Check that element isn't in the list
	if (!chunk->prev && !chunk->next && arena->free_large_chunks != chunk)
	{
		ft_printf("%p - %p : %zu bytes\n", (void*)chunk, (void*)chunk + chunk->size, chunk->size);
		return (chunk->size);
	}
	return (0);
}

// print heaps
static void	ft_mal_show_heaps(t_s_ft_mal_state *arena)
{
	t_s_ft_mal_heap_info	*heap;
	size_t					total_empty_size;

	heap = arena->heaps;
	total_empty_size = 0;
	while (heap)
	{
		if (heap->heap_type == FT_MAL_TINY_HEAP_TYPE)
			total_empty_size += ft_mal_show_tiny_heap(arena, heap);
		else if (heap->heap_type == FT_MAL_SMALL_HEAP_TYPE)
			total_empty_size += ft_mal_show_small_heap(arena, heap);
		else if (heap->heap_type == FT_MAL_LARGE_HEAP_TYPE)
			total_empty_size += ft_mal_show_large_heap(arena, heap);

		heap = heap->next;
	}
	ft_printf("Total : %zu bytes\n", total_empty_size);
}

void		show_alloc_mem(void)
{
	t_s_ft_mal_state	*arena;

	// get head of arenas list
	arena = ft_mal_get_arenas_list();
	
	while (arena)
	{
		// lock mutex
		FT_MAL_MUTEX_LOCK(&arena->mutex);

		ft_printf("Arena ID: %d\n", arena->arena_id);

		// sort heaps
		ft_mal_sort_heaps(&arena->heaps);

		// print heaps
		ft_mal_show_heaps(arena);
		
		// unlock mutex after showing info
		FT_MAL_MUTEX_UNLOCK(&arena->mutex);
	
		arena = arena->next;
	}	
}
