/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_mal_arena.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aashara- <aashara-@student.21-school.ru    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 16:23:18 by aashara-          #+#    #+#             */
/*   Updated: 2021/10/21 16:23:18 by aashara-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_mal_precomp.h"
#include "ft_mal_heap.h"

t_s_ft_mal_heap_info	*ft_mal_new_heap(size_t alloc_size)
{
	// t_s_ft_mal_heap_info	*heap_info;

	// if (size + top_pad < HEAP_MIN_SIZE)
	// 	size = HEAP_MIN_SIZE;
	// else if (size + top_pad <= HEAP_MAX_SIZE)
	// 	size += top_pad;
	// else if (size > HEAP_MAX_SIZE)
	// 	return (NULL);
	// else
	// 	size = HEAP_MAX_SIZE;
	// size = (size + page_mask) & ~page_mask;
	(void)alloc_size;
	return (NULL);
}

int					ft_mal_free_heap(t_s_ft_mal_heap_info *heap)
{
	if (heap)
		return (munmap((void*)heap, heap->total_size));// return function status
	return (FT_MAL_SUCCESS);
}
