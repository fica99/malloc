/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aashara- <aashara-@student.21-school.ru    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/02 17:52:28 by aashara-          #+#    #+#             */
/*   Updated: 2021/11/02 17:58:12 by aashara-         ###   ########.fr       */
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
*********** All memory functions header ********************
*/
#include "ft_mal_memory.h"

void	*calloc(size_t num, size_t size)
{
	void	*ptr;

	// allocate memory
	ptr = malloc(size * num);

	// if pointer is allocated, we should set all bytes to 0
	if (ptr)
		ft_bzero(ptr, size * num);

	return (ptr);
}
