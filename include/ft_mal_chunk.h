/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_mal_chunk.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 17:00:31 by marvin            #+#    #+#             */
/*   Updated: 2021/10/19 17:00:31 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_MAL_CHUNK_H
# define FT_MAL_CHUNK_H

/*
*************************** Macroses *****************************************
*/

/*
********* Chunk sizes ************
*/
# define FT_MAL_TINY_CHUNK_MIN_ALLOC_SIZE 1
# define FT_MAL_TINY_CHUNK_MAX_ALLOC_SIZE 256
# define FT_MAL_SMALL_CHUNK_MIN_ALLOC_SIZE (FT_MAL_TINY_CHUNK_MAX_ALLOC_SIZE + 1)
# define FT_MAL_SMALL_CHUNK_MAX_ALLOC_SIZE 2048
# define FT_MAL_LARGE_CHUNK_MIN_ALLOC_SIZE (FT_MAL_SMALL_CHUNK_MAX_ALLOC_SIZE + 1)


/*
*************************** Structures **************************************
*/

/*
******** chunk representation ****
*/
typedef struct				s_ft_mal_chunk
{
	/* Size in bytes, excluding header */
	size_t					size;

	/* double links -- used only if free. */
	
	/* Next chunk. */
	struct s_ft_mal_chunk*	next;
	
	/* Previous chunk. */
	struct s_ft_mal_chunk*	prev;
}							t_s_ft_mal_chunk;

# define FT_MAL_CHUNK_SIZE (sizeof(t_s_ft_mal_chunk))
# define FT_MAL_CHUNK_SHIFT(start) ((void*)start + FT_MAL_CHUNK_SIZE)
# define FT_MAL_CHUNK_BACK_SHIFT(start) ((void*)start - FT_MAL_CHUNK_SIZE)
#endif
