/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_mal_print.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aashara- <aashara-@student.21-school.ru    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/05 11:30:51 by aashara-          #+#    #+#             */
/*   Updated: 2021/11/05 16:10:18 by aashara-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_MAL_PRINT_H
# define FT_MAL_PRINT_H

/*
*************************** System headers ************************************
*/

/*
************* size_t ****************************
*/
#include <stddef.h>


/*
*************************** Macroses  *****************************************
*/

/*
*********** Bases ***************************
*/
# define FT_MAL_HEX_BASE 16
# define FT_MAL_DEC_BASE 10 


/*
************ ft_mal_print.c *********************
*/
void	ft_mal_putnbr_base(size_t nb, size_t base, size_t length, bool is_prefix);
void	ft_mal_print(const char *str1, size_t nb, size_t base, const char *str2);
#endif
