/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_mal_chunk.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aashara- <aashara-@student.21-school.ru    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/30 15:06:02 by aashara-          #+#    #+#             */
/*   Updated: 2021/10/30 15:20:21 by aashara-         ###   ########.fr       */
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
*********** Chunk header ********************
*/
#include "ft_mal_chunk.h"

// add chunk to start of the chunks list
void	ft_mal_add_chunk_to_list(t_s_ft_mal_chunk **head, t_s_ft_mal_chunk *chunk)
{
	// assign next element
	chunk->next = *head;
	
	// assign previous element
	if (*head)
		(*head)->prev = chunk;
	
	// add element to the start of the list
	*head = chunk;
}

// remove chunk from the list
void	ft_mal_remove_chunk_from_list(t_s_ft_mal_chunk **head, t_s_ft_mal_chunk *chunk)
{
	t_s_ft_mal_chunk	*next;
	t_s_ft_mal_chunk	*prev;
	
	next = chunk->next;
	prev = chunk->prev;

	// update next element
	if (next)
		next->prev = prev;

	// update previous element
	if (prev)
		prev->next = next;

	// if chunk is head, remove it
	if (*head == chunk)
		*head = next;
}
