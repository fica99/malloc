/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_mal_arena.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aashara- <aashara-@student.21-school.ru    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 10:34:03 by aashara-          #+#    #+#             */
/*   Updated: 2021/10/21 10:34:03 by aashara-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_MAL_ARENA_H
# define FT_MAL_ARENA_H
/*
*************************** System headers ************************************
*/

/*
****************** pthread_mutex_t ********
****************** pthread_mutex_trylock **
****************** pthread_mutex_unlock ***
****************** pthread_mutex_init *****
****************** pthread_mutex_destroy **
*/
# include <pthread.h>


/*
*************************** Macroses  *****************************************
*/

/*
******************* Limits ***************************************
*/
# define FT_MAL_MAX_NB_ARENAS 2
/*
******************* Arenas mutex operations **********************
*/
# define FT_MAL_ARENA_TRYLOCK(mutex_ptr) (pthread_mutex_trylock(mutex_ptr))
# define FT_MAL_ARENA_UNLOCK(mutex_ptr) (pthread_mutex_unlock(mutex_ptr))


/*
*************************** Structures ****************************************
*/

/*
******************* Malloc state (Arena header) *******************
*/
typedef struct				s_ft_mal_state
{
	/* Thread access. */
	pthread_mutex_t			mutex;

	/* ID of arena */
	size_t					arena_id;
	
	/* Linked list */
	struct s_ft_mal_state	*next;
}							t_s_ft_mal_state;

# define FT_MAL_STATE_SIZE (sizeof(t_s_ft_mal_state))
# define FT_MAL_STATE_SHIFT(start) ((void*)start + FT_MAL_STATE_SIZE)
/*
*************************** Functions ****************************************
*/

/*
******************** ft_mal_arena.c *****************************
*/
t_s_ft_mal_state			*ft_mal_get_available_arena(size_t alloc_size);
#endif
