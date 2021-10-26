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
*************************** Structures ****************************************
*/

/*
******** malloc_chunk (Chunk representations) ********
*/
typedef struct				s_ft_mal_chunk
{
	size_t					prev_size;		/* Size of previous chunk (if free).  */
	size_t					size;			/* Size in bytes, including overhead. */

	struct s_ft_mal_chunk*	fd;				/* double links -- used only if free. */
	struct s_ft_mal_chunk*	bk;
}							t_s_ft_mal_chunk;
#endif
