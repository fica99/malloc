/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_mal_chunk.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aashara- <aashara-@student.21-school.ru    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/30 15:06:02 by aashara-          #+#    #+#             */
/*   Updated: 2021/10/31 21:48:32 by aashara-         ###   ########.fr       */
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
	chunk->prev = NULL;
	
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

	// remove prev and next links from chunk
	chunk->next = NULL;
	chunk->prev = NULL;
}

// merge continious chunks in one
void	ft_mal_merge_chunks(t_s_ft_mal_chunk *chunks)
{
	t_s_ft_mal_chunk	*current;
	t_s_ft_mal_chunk	*next;

	current = chunks;
	while (current)
	{
		// next element
		next = current->next;
		if (next)
		{
			// if blocks are connected, we can merge it
			if (FT_MAL_CHUNK_SHIFT(current) + current->size == (void*)next)
			{
				// merge size and header
				current->size += (next->size + sizeof(t_s_ft_mal_chunk));
				
				// remove merged element from list
				// we sending pointer to head, but head element cannot be removed
				ft_mal_remove_chunk_from_list(&chunks, next);
				
				continue;
			}
		}
		current = next;
	}
}

// sort chunks by adress
void	ft_mal_sort_list(t_s_ft_mal_chunk **head)
{
	t_s_ft_mal_chunk	*current_chunk;
	t_s_ft_mal_chunk	*next;
	t_s_ft_mal_chunk	*tmp;

	// start from first element
	current_chunk = *head;
	while (current_chunk)
	{
		next = current_chunk->next;
		if (next)
		{
			// compare with next element
			if ((void*)current_chunk > (void*)next)
			{
				// swap elements in list
				tmp = next->next;
				next->prev = current_chunk->prev;
				next->next = current_chunk;
				current_chunk->prev = next;
				current_chunk->next = tmp;
				
				// check one element before for swapping adresses
				if (next->prev)
				{
					current_chunk = next->prev;
					continue ;
				}
				else
				{
					// update head after swapping elements
					*head = next;
				}
			}
		}
		current_chunk = next;
	}
}
