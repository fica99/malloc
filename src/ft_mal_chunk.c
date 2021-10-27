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

/*
*************************** User headers ************************************
*/

/*
*********** Precompiled header **************
*/
#include "ft_mal_precomp.h"

/*
*********** Chunk header **************
*/
#include "ft_mal_chunk.h"


/*
*************************** Functions ****************************************
*/

t_e_ft_mal_chunk_type		ft_mal_get_chunk_type_by_alloc_size(size_t size)
{
	if (size >= FT_MAL_TINY_CHUNK_MIN_ALLOC_SIZE && size <= FT_MAL_TINY_CHUNK_MAX_ALLOC_SIZE)
		return (FT_MAL_TINY_CHUNK_TYPE);
	else if (size >= FT_MAL_SMALL_CHUNK_MIN_ALLOC_SIZE && size <= FT_MAL_SMALL_CHUNK_MAX_ALLOC_SIZE)
		return (FT_MAL_SMALL_CHUNK_TYPE);
	else if (size >= FT_MAL_LARGE_CHUNK_MIN_ALLOC_SIZE)
		return (FT_MAL_LARGE_CHUNK_TYPE);
	return (FT_MAL_UNKNOWN_CHUNK_TYPE);
}

