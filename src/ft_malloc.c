/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_malloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aashara- <aashara-@student.21-school.ru    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 15:58:58 by aashara-          #+#    #+#             */
/*   Updated: 2021/10/21 15:58:58 by aashara-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_mal_precomp.h"
#include "ft_mal_memory.h"
#include "ft_mal_arena.h"

void	*malloc(size_t size)
{
	t_s_ft_mal_state	*arena;

	arena = ft_mal_get_available_arena(size);

	// error happened
	if (!arena)
		return (NULL);
	
	return (NULL);	
}
