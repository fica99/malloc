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
*************************** Macroses ********************************************
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
*************************** Structures ****************************************
*/

/*
******** malloc_chunk (chunk representations)
*/
typedef struct				s_ft_mal_chunk
{
	size_t					size;			/* Size in bytes, including overhead. */

	struct s_ft_mal_chunk*	fd;				/* double links -- used only if free. */
	struct s_ft_mal_chunk*	bk;
}							t_s_ft_mal_chunk;
# define FT_MAL_CHUNK_SIZE (sizeof(t_s_ft_mal_chunk))
# define FT_MAL_CHUNK_SHIFT(start) ((void*)start + FT_MAL_CHUNK_SIZE)
# define FT_MAL_CHUNK_BACK_SHIFT(start) ((void*)start - FT_MAL_CHUNK_SIZE)
#endif
