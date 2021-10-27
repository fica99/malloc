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
*************************** Enums **********************************************
*/

/*
******** Chunk type **************
*/
typedef enum	e_ft_mal_chunk_type
{
	FT_MAL_UNKNOWN_CHUNK_TYPE,
	FT_MAL_TINY_CHUNK_TYPE,
	FT_MAL_SMALL_CHUNK_TYPE,
	FT_MAL_LARGE_CHUNK_TYPE
}				t_e_ft_mal_chunk_type;


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
# define FT_MAL_CHUNK_BACK_SHIFT(start) ((void*)start - FT_MAL_CHUNK_SIZE)


/*
*************************** Functions ****************************************
*/

/*
********* ft_mal_chunk.c ************
*/
t_e_ft_mal_chunk_type		ft_mal_get_chunk_type_by_alloc_size(size_t size);
#endif
