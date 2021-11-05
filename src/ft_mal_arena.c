/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_mal_arena.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aashara- <aashara-@student.21-school.ru    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 16:23:18 by aashara-          #+#    #+#             */
/*   Updated: 2021/10/31 22:45:17 by aashara-         ###   ########.fr       */
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
*********** Arena header ********************
*/
#include "ft_mal_arena.h"


/*
*************************** System headers ************************************
*/

/*
*********** gettid **************************
*********** syscall *************************
*/
#define _GNU_SOURCE
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>


/*
*************************** Macroses  *****************************************
*/

/*
*********** Limits ***************************
*/
# define FT_MAL_MAX_NB_ARENAS 8		// number of arenas
# define FT_MAL_MAX_THREADS 410669	// max number of threads


/*
*************************** Global variables ********************************
*/

/*
*********** Pointer to arenas ****************
*/
t_s_ft_mal_state	*g_ft_arena = NULL;

// create new arena
static t_s_ft_mal_state	*ft_mal_new_arena(void)
{
	t_s_ft_mal_state		*arena;
	t_s_ft_mal_heap_info	*heap_info;

	// initialize new heap
	heap_info = ft_mal_new_heap(NULL, FT_MAL_TINY_HEAP_TYPE, 0);
	
	// error check
	if (!heap_info)
		return (NULL);
	
	arena = heap_info->ar_ptr;
	
	// initialize mutex for this arena
	if (pthread_mutex_init(&arena->mutex, NULL) != FT_MAL_SUCCESS)
	{
		// error happened on mutex initialization
		ft_mal_free_heap(&arena->heaps, heap_info);
		return (NULL);
	}
	
	return (arena);
}

// add arena to start of the arenas list
static void				ft_mal_add_arena_to_list(t_s_ft_mal_state **head, t_s_ft_mal_state *arena)
{
	// assign next element
	arena->next = *head;
	
	// add element to the start of the list
	*head = arena;
}

// is arena used by another thread (lock mutex)
static bool				ft_mal_is_available_arena(t_s_ft_mal_state *arena)
{
	if (!arena)
		return (false);
	if (FT_MAL_MUTEX_TRYLOCK(&arena->mutex) == FT_MAL_SUCCESS)
		return (true);
	return (false);
}

// find arena that is not in use by other thread or create new arena (locking mutex)
static t_s_ft_mal_state	*ft_mal_find_available_arena(t_s_ft_mal_state **arena)
{
	t_s_ft_mal_state	*res_arena;

	res_arena = *arena;
	while (true)
	{
		// create new arena if limit is not reached and there is no available arena
		if (!res_arena && (!*arena || (*arena)->arena_id < FT_MAL_MAX_NB_ARENAS))
		{
			// create new arena
			res_arena = ft_mal_new_arena();

			// error check
			if (!res_arena)
				return (NULL);

			// assign id to new arena
			res_arena->arena_id = ((*arena) ? (*arena)->arena_id + 1 : 0);
			
			// add arena to the start of the list
			ft_mal_add_arena_to_list(arena, res_arena);

			// lock mutex
			FT_MAL_MUTEX_LOCK(&res_arena->mutex);

			break ;
		}
		
		// continue loop from the beginning if all arenas are locked
		if (!res_arena)
		{
			res_arena = *arena;
			continue ;
		}
		else if (*arena && (*arena)->arena_id >= FT_MAL_MAX_NB_ARENAS)
		{
			// locking mutex if return value is true
			if (ft_mal_is_available_arena(res_arena))
				break ;
		}
		res_arena = res_arena->next;

	}

	return (res_arena);
}

// checks is this pointer was allocated in this arena
static bool	ft_mal_is_arenas_ptr(t_s_ft_mal_state *arena, void *ptr)
{
	t_s_ft_mal_heap_info	*heap;
	t_e_ft_mal_heap_type	heap_type;


	// maybe arena is not created
	if (!arena)
		return (false);

	// determine heap type by ptr
	heap_type = ft_mal_get_heap_type_by_ptr(ptr);
	
	// lock mutex for iterating throw arena
	FT_MAL_MUTEX_LOCK(&arena->mutex);

	// search throw all heaps
	heap = arena->heaps;
	while (heap)
	{
		// is heap type equal
		if (heap_type == heap->heap_type)
		{
			// is pointer inside heap
			if (ptr > (void*)heap && (ptr - (void*)heap) <= (long int)heap->total_size)
				return (true);
		}
		heap = heap->next;
	}

	// unlock mutex, if arena is not found
	FT_MAL_MUTEX_UNLOCK(&arena->mutex);
	return (false);
}

// check or save arena id for thread id
static bool				ft_mal_arena_tid(size_t arena_id, pid_t tid, bool is_writing)
{
	static bool	arenas_tid[FT_MAL_MAX_NB_ARENAS][FT_MAL_MAX_THREADS];
	bool		res;

	if (is_writing)
		arenas_tid[arena_id % FT_MAL_MAX_NB_ARENAS][tid % FT_MAL_MAX_THREADS] = true;
	res = arenas_tid[arena_id % FT_MAL_MAX_NB_ARENAS][tid % FT_MAL_MAX_THREADS];
	return (res);
}

// get saved arena for current thread id
static t_s_ft_mal_state	*ft_mal_get_saved_arena_tid(t_s_ft_mal_state *arena, pid_t tid)
{
	while (arena)
	{
		if (ft_mal_arena_tid(arena->arena_id, tid, false))
			return (arena);
		arena = arena->next;
	}
	return (NULL);
}

// get thread id
static pid_t			gettid(void)
{
	pid_t tid = (pid_t)syscall(SYS_gettid);
	return tid;
}

// get arena by pointer (locking mutex)(search throw all arenas and heaps)(first search in current thread arena)
t_s_ft_mal_state		*ft_mal_get_arena_by_ptr(void *ptr)
{
	t_s_ft_mal_state	*current_tid_arena;
	t_s_ft_mal_state	*res_arena;
	pid_t				tid;

	// get current thread id
	tid = gettid();

	// find saved arena for current thread id
	current_tid_arena = ft_mal_get_saved_arena_tid(g_ft_arena, tid);

	// check is the memory allocated in the same thread (locking mutex)
	if (ft_mal_is_arenas_ptr(current_tid_arena, ptr))
		return (current_tid_arena);

	// search throw all arenas
	res_arena = g_ft_arena;
	while (res_arena)
	{
		if (res_arena != current_tid_arena
			&& ft_mal_is_arenas_ptr(res_arena, ptr)) //(locking mutex)
			return (res_arena);
		res_arena = res_arena->next;
	}
	return (NULL);
}

// get available arena for current thread (locking mutex)
t_s_ft_mal_state		*ft_mal_get_available_arena(void)
{
	pid_t				tid;
	t_s_ft_mal_state	*res_arena;

	// get current thread id
	tid = gettid();

	// find saved arena for current thread id
	res_arena = ft_mal_get_saved_arena_tid(g_ft_arena, tid);
	
	// find empty arena or create new, if there is no saved arena for current thread
	if (!res_arena)
	{
		// locking mutex
		res_arena = ft_mal_find_available_arena(&g_ft_arena);
		
		// save arena for current thread id
		ft_mal_arena_tid(res_arena->arena_id, tid, true);
	}
	else
		FT_MAL_MUTEX_LOCK(&res_arena->mutex); // locking mutex
	
	return (res_arena);
}

// return arenas global variable head (get list of all arenas)
t_s_ft_mal_state			*ft_mal_get_arenas_list(void)
{
	return (g_ft_arena);
}
