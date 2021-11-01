/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_mal_arena.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aashara- <aashara-@student.21-school.ru    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 10:34:03 by aashara-          #+#    #+#             */
/*   Updated: 2021/10/30 16:08:48 by aashara-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_MAL_ARENA_H
# define FT_MAL_ARENA_H

/*
*************************** User headers ************************************
*/

/*
*********** Chunk ***************************
*/
# include "ft_mal_chunk.h"

/*
*********** Heaps ***************************
*/
#include "ft_mal_heap.h"


/*
************************** Structures ***************************************
*/

/*
********** Malloc state (Arena header) ******
*/
typedef struct s_ft_mal_heap_info t_s_ft_mal_heap_info;

typedef struct				s_ft_mal_state
{
	/* Thread access. */
	pthread_mutex_t			mutex;

	/* ID of arena */
	size_t					arena_id;

	/* Heaps of arena */
	t_s_ft_mal_heap_info	*heaps;	// needs only for show_alloc_mem();
	
	/* Empty tiny chunks */
	t_s_ft_mal_chunk		*free_tiny_chunks;	// use pool allocator

	/* Empty small chunks */
	t_s_ft_mal_chunk		*free_small_chunks;	// use free list allocator
	
	/* Empty large chunks */
	t_s_ft_mal_chunk		*free_large_chunks;	// using for empty large chunks

	/* Linked list */
	struct s_ft_mal_state	*next;
}							t_s_ft_mal_state;

# define FT_MAL_STATE_SIZE (sizeof(t_s_ft_mal_state))
# define FT_MAL_STATE_SHIFT(start) ((void*)start + FT_MAL_STATE_SIZE)


/*
*************************** Functions ****************************************
*/

/*
************* ft_mal_arena.c ******************
*/
t_s_ft_mal_state			*ft_mal_get_arena_by_ptr(void *ptr);
t_s_ft_mal_state			*ft_mal_get_available_arena(void);
void						*ft_mal_allocate_memory(t_s_ft_mal_state *arena,
								size_t alloc_size);
void						ft_mal_free_memory(t_s_ft_mal_state *arena,
								void *ptr);
void						*ft_mal_realloc_memory(t_s_ft_mal_state *arena,
								void *ptr, size_t size);

/*
*************************** Global variables ********************************
*/
extern t_s_ft_mal_state		*g_ft_arena;
#endif
