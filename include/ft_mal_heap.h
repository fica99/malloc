/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_mal_heap.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aashara- <aashara-@student.21-school.ru    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 11:48:42 by aashara-          #+#    #+#             */
/*   Updated: 2021/10/30 16:26:34 by aashara-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_MAL_HEAP_H
# define FT_MAL_HEAP_H

/*
*************************** User headers **************************************
*/

/*
********* Malloc state ******************************
*/
# include "ft_mal_arena.h"

/*
*************************** Enums *********************************************
*/

/*
******** Heap type **************
*/
typedef struct s_ft_mal_state t_s_ft_mal_state;

typedef enum	e_ft_mal_heap_type
{
	FT_MAL_TINY_HEAP_TYPE,
	FT_MAL_SMALL_HEAP_TYPE,
	FT_MAL_LARGE_HEAP_TYPE
}				t_e_ft_mal_heap_type;

/*
*************************** Structures ****************************************
*/

/*
******************* Heap info (Heap header) *******************
*/
typedef struct					s_ft_mal_heap_info
{
	/* Arena for this heap. */
	t_s_ft_mal_state			*ar_ptr;

	/* Next heap. */
	struct s_ft_mal_heap_info	*next;

	/* Previous heap. */
	struct s_ft_mal_heap_info	*prev;

	/* Total size of the heap.(include current header and other headers) */
	size_t						total_size;

	/* Heap type*/
	t_e_ft_mal_heap_type		heap_type;
}								t_s_ft_mal_heap_info;

static const size_t FT_MAL_HEAP_INFO_SIZE = sizeof(t_s_ft_mal_heap_info);
# define FT_MAL_HEAP_INFO_SHIFT(start) ((void*)start + FT_MAL_HEAP_INFO_SIZE)
# define FT_MAL_HEAP_INFO_BACK_SHIFT(start) ((void*)start - FT_MAL_HEAP_INFO_SIZE)

/*
*************************** Functions ****************************************
*/

/*
******************** ft_mal_heap.c *****************************
*/
t_s_ft_mal_heap_info			*ft_mal_new_heap(t_s_ft_mal_state *arena, 
									t_e_ft_mal_heap_type heap_type, size_t large_chunk_size);
int								ft_mal_free_heap(t_s_ft_mal_heap_info **head, t_s_ft_mal_heap_info *heap);
t_e_ft_mal_heap_type			ft_mal_get_heap_type_by_alloc_size(size_t alloc_size);
t_e_ft_mal_heap_type			ft_mal_get_heap_type_by_ptr(void *ptr);
void							ft_mal_sort_heaps(t_s_ft_mal_heap_info **head);
#endif

