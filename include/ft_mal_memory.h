/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_mal_memory.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aashara- <aashara-@student.21-school.ru    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 15:53:11 by aashara-          #+#    #+#             */
/*   Updated: 2021/10/21 15:53:11 by aashara-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_MAL_MEMORY_H
# define FT_MAL_MEMORY_H

/*
******************************** System headers *******************************
*/

/*
******* size_t ************************
*/
#include <stddef.h>


/*
******* ft_free.c **********************
*/
void	free(void *ptr);

/*
******* ft_malloc.c ********************
*/
void	*malloc(size_t size);

/*
******* ft_realloc.c *******************
*/
void	*realloc(void *ptr, size_t size);

/*
******* ft_reallocf.c ******************
*/
void	*reallocf(void *ptr, size_t size);

/*
******* ft_calloc.c ********************
*/
void	*calloc(size_t num, size_t size);

/*
******* ft_mal_show_alloc_mem.c ********
*/
void	show_alloc_mem(void);

#endif
