/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_mal_show_alloc_mem.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aashara- <aashara-@student.21-school.ru    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 15:58:58 by aashara-          #+#    #+#             */
/*   Updated: 2021/11/04 17:34:11 by aashara-         ###   ########.fr       */
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
*********** Chunk header ********************
*/
#include "ft_mal_chunk.h"

/*
*********** libft ***************************
*/
#include "libft.h"

// putnbr for different bases
static void	ft_mal_putnbr_base(size_t nb, size_t base)
{
	// alphabet
	static const char	*alphabet = "0123456789ABCDEFGHIJKLMNOPQRSTUIVWXYZ";
	static size_t		alphabet_length;
	size_t				res;
	
	// initialize length
	if (!alphabet_length)
		alphabet_length = ft_strlen(alphabet);
		
	// invalid base
	if (base < 2 || base > alphabet_length)
		return ;
		
	res = nb / base;
	if (res)
		ft_mal_putnbr_base(res, base);
	else
	{
		if (base == 16)
			ft_putstr("0x");
	}
	
	if (write(1, alphabet + (nb % base), 1) == FT_MAL_FAILURE)
		return ;
}



// show tiny heap
static size_t	ft_mal_show_tiny_heap(t_s_ft_mal_state *arena, t_s_ft_mal_heap_info *heap)
{
	t_s_ft_mal_chunk		*chunk;
	size_t					i;
	size_t					total_size;
	
	// print heap header address
	ft_putstr("TINY : ");
	ft_mal_putnbr_base((size_t)heap, 16);
	ft_putstr("\n");
	
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
			// print start of the chunk address
			ft_mal_putnbr_base((size_t)chunk, 16);
			
			ft_putstr(" - ");
			
			// print end of the chunk address
			ft_mal_putnbr_base((size_t)(chunk + chunk->size), 16);
			
			ft_putstr(" : ");
			
			// print size of the chunk
			ft_mal_putnbr_base(chunk->size, 10);
			
			ft_putstr(" bytes\n");
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
	ft_putstr("SMALL : ");
	ft_mal_putnbr_base((size_t)heap, 16);
	ft_putstr("\n");

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
			// print start of the chunk address
			ft_mal_putnbr_base((size_t)chunk, 16);
			
			ft_putstr(" - ");
			// print end of the chunk address
			ft_mal_putnbr_base((size_t)(chunk + chunk->size), 16);
			
			ft_putstr(" : ");
			// print size of the chunk
			ft_mal_putnbr_base(chunk->size, 10);
			
			ft_putstr(" bytes\n");
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
	ft_putstr("LARGE : ");
	ft_mal_putnbr_base((size_t)heap, 16);
	ft_putstr("\n");
	
	// shift heap
	chunk = FT_MAL_HEAP_INFO_SHIFT(heap);

	// Check that element isn't in the list
	if (!chunk->prev && !chunk->next && arena->free_large_chunks != chunk)
	{
		// print start of the chunk address
		ft_mal_putnbr_base((size_t)chunk, 16);
			
		ft_putstr(" - ");
		
		// print end of the chunk address
		ft_mal_putnbr_base((size_t)(chunk + chunk->size), 16);
			
		ft_putstr(" : ");
			
		// print size of the chunk
		ft_mal_putnbr_base(chunk->size, 10);
			
		ft_putstr(" bytes\n");
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
	ft_putstr("Total : ");
	ft_mal_putnbr_base(total_empty_size, 10);
	ft_putstr(" bytes\n");
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

		// print arena header
		ft_putstr("Arena ID : ");
		ft_mal_putnbr_base(arena->arena_id, 10);
		ft_putstr("\n");

		// sort heaps
		ft_mal_sort_heaps(&arena->heaps);

		// print heaps
		ft_mal_show_heaps(arena);
		
		// unlock mutex after showing info
		FT_MAL_MUTEX_UNLOCK(&arena->mutex);
	
		arena = arena->next;
	}	
}
