/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_mal_show_alloc_mem_ex.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aashara- <aashara-@student.21-school.ru    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 15:58:58 by aashara-          #+#    #+#             */
/*   Updated: 2021/11/05 16:54:47 by aashara-         ###   ########.fr       */
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
	size_t			i;
	size_t			j;
	unsigned char	symb;
	
	// print start of the chunk address
	ft_mal_putnbr_base((size_t)FT_MAL_CHUNK_SHIFT(chunk), FT_MAL_HEX_BASE, 0, true);

	// print end of the chunk address
	ft_mal_print(" - ", (size_t)(FT_MAL_CHUNK_SHIFT(chunk) + chunk->size), FT_MAL_HEX_BASE, " : ");

	// print size of the chunk
	ft_mal_print(NULL, chunk->size, FT_MAL_DEC_BASE, " bytes\n");

	i = 0;
	// print hexa dump
	while (i < chunk->size)
	{
		ft_mal_putnbr_base((size_t)(FT_MAL_CHUNK_SHIFT(chunk) + i), FT_MAL_HEX_BASE, 0, true);
		j = 0;
		while (j < FT_MAL_HEX_BASE && i + j < chunk->size)
		{
			symb = *(unsigned char*)((void*)FT_MAL_CHUNK_SHIFT(chunk) + i + j);
			ft_putchar(' ');
			if (symb > FT_MAL_HEX_BASE)
				ft_mal_putnbr_base((size_t)symb, FT_MAL_HEX_BASE, 0, false);
			else
				ft_mal_putnbr_base((size_t)symb, FT_MAL_HEX_BASE, 1, false);
			++j;
		}
		
		i += j;
		ft_putchar('\n');
	}
}

// show tiny heap
static size_t	ft_mal_show_tiny_heap(t_s_ft_mal_state *arena, t_s_ft_mal_heap_info *heap)
{
	t_s_ft_mal_chunk		*chunk;
	size_t					i;
	size_t					total_size;
	
	// print heap header address
	ft_mal_print("Tiny heap : ", (size_t)heap, FT_MAL_HEX_BASE, "\n");
		
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
	ft_mal_print("Small heap : ", (size_t)heap, FT_MAL_HEX_BASE, "\n");

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
	ft_mal_print("Large heap : ", (size_t)heap, FT_MAL_HEX_BASE, "\n");
	
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
	size_t					total_size;
	size_t					size;
	size_t					total_allocated_size;
	size_t					nb_heaps;

	heap = arena->heaps;
	total_allocated_size = 0;
	total_size = 0;
	nb_heaps = 0;
	while (heap)
	{
		// print heap total size
		ft_mal_print("Heap size : ", heap->total_size, FT_MAL_DEC_BASE, " bytes\n");

		size = 0;
		if (heap->heap_type == FT_MAL_TINY_HEAP_TYPE)
			size = ft_mal_show_tiny_heap(arena, heap);
		else if (heap->heap_type == FT_MAL_SMALL_HEAP_TYPE)
			size = ft_mal_show_small_heap(arena, heap);
		else if (heap->heap_type == FT_MAL_LARGE_HEAP_TYPE)
			size = ft_mal_show_large_heap(arena, heap);

		// print heap allocated size
		ft_mal_print("Allocated size : ", size, FT_MAL_DEC_BASE, " bytes\n");

		total_allocated_size += size;
		total_size += heap->total_size;
		++nb_heaps;
		
		heap = heap->next;
	}
	// print arena info
	ft_mal_print("Total allocated size : ", total_allocated_size, FT_MAL_DEC_BASE, " bytes\n");
	ft_mal_print("Total size : ", total_size, FT_MAL_DEC_BASE," bytes\n");
	ft_mal_print("Number of heaps : ", nb_heaps, FT_MAL_DEC_BASE, "\n");
}

void		show_alloc_mem_ex(void)
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
		ft_mal_print("Arena ID : ", arena->arena_id, FT_MAL_DEC_BASE, "\n");

		// print heaps
		ft_mal_show_heaps(arena);

		ft_putstr("\n\n");

		// unlock mutex for writing
		FT_MAL_MUTEX_UNLOCK(&g_ft_mal_writing_mutex);

		// unlock mutex for arena
		FT_MAL_MUTEX_UNLOCK(&arena->mutex);
	
		arena = arena->next;
	}	
}
