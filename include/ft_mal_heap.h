/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_mal_heap.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aashara- <aashara-@student.21-school.ru    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 11:48:42 by aashara-          #+#    #+#             */
/*   Updated: 2021/10/21 11:48:42 by aashara-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_MAL_HEAP_H
# define FT_MAL_HEAP_H

/*
*************************** User headers ************************************
*/

/*
******************* Malloc state ******************************
*/
# include "ft_mal_arena.h"

/*
*************************** Macroses ********************************************
*/


/*
*************************** Structures ****************************************
*/

/*
******************* Heap info (Heap header) *******************
*/
typedef struct					s_ft_mal_heap_info
{
	t_s_ft_mal_state			*ar_ptr;	/* Arena for this heap. */
	struct s_ft_mal_heap_info	*next;		/* Next heap. */
	struct s_ft_mal_heap_info	*prev;		/* Previous heap. */
	size_t						total_size;	/* Total size of the heap. */
}								t_s_ft_mal_heap_info;

# define FT_MAL_HEAP_INFO_SIZE (sizeof(t_s_ft_mal_heap_info))
# define FT_MAL_HEAP_INFO_SHIFT(start) ((void*)start + FT_MAL_HEAP_INFO_SIZE)

/*
*************************** Functions ****************************************
*/
/*
******************** ft_mal_heap.c *****************************
*/
t_s_ft_mal_heap_info			*ft_mal_new_heap(size_t alloc_size, bool is_arena_included);
int								ft_mal_free_heap(t_s_ft_mal_heap_info *heap);
#endif

