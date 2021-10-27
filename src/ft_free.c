/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_malloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aashara- <aashara-@student.21-school.ru    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 15:58:58 by aashara-          #+#    #+#             */
/*   Updated: 2021/10/21 15:58:58 by aashara-         ###   ########.fr       */
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
*********** Chunk header ********************
*/
#include "ft_mal_chunk.h"

static void	ft_mal_remove_large_chunk_heap(t_s_ft_mal_chunk *chunk)
{

}

void		free(void *ptr)
{
	t_s_ft_mal_chunk		*chunk;
	t_e_ft_mal_chunk_type	chunk_type;
	
	if (ptr == NULL)
		return ;
	
	// backward shift to determine chunk
	chunk = FT_MAL_CHUNK_BACK_SHIFT(ptr);
	chunk_type = ft_mal_get_chunk_type_by_alloc_size(chunk->size);
	
	// if it is large chunk, remove heap
	if (chunk_type == FT_MAL_LARGE_CHUNK_TYPE)
		ft_mal_remove_large_chunk_heap(chunk);

}
