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
*********** Heap header *********************
*/
#include "ft_mal_heap.h"

/*
*********** Chunk struct ********************
*********** Chunk type **********************
*/
#include "ft_mal_chunk.h"


/*
*************************** System headers ************************************
*/

/*
******************* getpagesize *************************
*/
# include <unistd.h>

/*
******************* mmap ********************************
******************* munmap ******************************
*/
#include <sys/mman.h>

/*
******************* getrlimit ***************************
*/
#include <sys/time.h>
#include <sys/resource.h>


/*
*************************** Macroses ********************************************
*/

/*
*************************** Number of chunks in heap *********
*/
# define FT_MAL_NB_OF_TINY_CHUNKS 240
# define FT_MAL_NB_OF_SMALL_CHUNKS 120


static size_t			ft_mal_get_heap_total_size(size_t alloc_size, bool is_arena_included)
{
	size_t					total_size;
	size_t					page_size;
	size_t					nb_pages;
	t_e_ft_mal_chunk_type	chunk_type;

	// determine pagesize
	page_size = getpagesize();

	// heap metadata
	total_size = sizeof(t_s_ft_mal_heap_info);

	// arena metadata
	if (is_arena_included == true)
		total_size += sizeof(t_s_ft_mal_state);

	// check heap type
	chunk_type = ft_mal_get_chunk_type_by_alloc_size(alloc_size);
	
	if (chunk_type == FT_MAL_LARGE_CHUNK_TYPE)
		total_size += (alloc_size + sizeof(t_s_ft_mal_chunk));
	else if (chunk_type == FT_MAL_SMALL_CHUNK_TYPE)
		total_size += ((FT_MAL_SMALL_CHUNK_MAX_ALLOC_SIZE + sizeof(t_s_ft_mal_chunk)) * FT_MAL_NB_OF_SMALL_CHUNKS);
	else if (chunk_type == FT_MAL_TINY_CHUNK_TYPE)
		total_size += ((FT_MAL_TINY_CHUNK_MAX_ALLOC_SIZE + sizeof(t_s_ft_mal_chunk)) * FT_MAL_NB_OF_TINY_CHUNKS);

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

t_s_ft_mal_heap_info	*ft_mal_new_heap(size_t alloc_size, bool is_arena_included)
{
	t_s_ft_mal_heap_info	*heap_info;
	t_s_ft_mal_chunk		*first_chunk;
	size_t					total_size;
	
	// determine heap size to allocate
	total_size = ft_mal_get_heap_total_size(alloc_size, is_arena_included);
	
	// check enough memory
	if (total_size > ft_mal_get_memory_limit())
		return (NULL);

	// allocate heap
	heap_info = (t_s_ft_mal_heap_info *)mmap(NULL, total_size,
		PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);

	// error check
	if (heap_info == MAP_FAILED)
		return (NULL);

	// initialize heap info
	ft_bzero((void*)heap_info, sizeof(t_s_ft_mal_heap_info));
	heap_info->total_size = total_size;
	heap_info->ar_ptr = (is_arena_included ? FT_MAL_HEAP_INFO_SHIFT(heap_info) : NULL);

	// initialize first chunk
	first_chunk = (is_arena_included ? FT_MAL_STATE_SHIFT(heap_info->ar_ptr) : FT_MAL_HEAP_INFO_SHIFT(heap_info));
	ft_bzero((void*)first_chunk, sizeof(t_s_ft_mal_chunk));
	first_chunk->size = total_size - (first_chunk - heap_info);

	return (heap_info);
}

int					ft_mal_free_heap(t_s_ft_mal_heap_info *heap)
{
	if (heap)
		return (munmap((void*)heap, heap->total_size)); // return function status
	return (FT_MAL_SUCCESS);
}
