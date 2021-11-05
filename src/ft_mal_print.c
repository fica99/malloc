/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_mal_print.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aashara- <aashara-@student.21-school.ru    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/05 11:34:49 by aashara-          #+#    #+#             */
/*   Updated: 2021/11/05 17:00:27 by aashara-         ###   ########.fr       */
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
*********** Print functions header **********
*/
#include "ft_mal_print.h"

/*
*********** libft ***************************
*/
#include "libft.h"


// putnbr for different bases
void	ft_mal_putnbr_base(size_t nb, size_t base, size_t length, bool is_prefix)
{
	// alphabet
	static const char	*alphabet = "0123456789ABCDEFGHIJKLMNOPQRSTUIVWXYZ";
	static size_t		alphabet_length;
	size_t				res;
	
	// initialize length
	if (!alphabet_length)
		alphabet_length = ft_strlen(alphabet);
		
	// invalid base
	if (base < 2 || base > alphabet_length)
		return ;
		
	res = nb / base;
	if (res)
		ft_mal_putnbr_base(res, base, length, is_prefix);
	else
	{
		if (base == FT_MAL_HEX_BASE && is_prefix)
			ft_putstr("0x");
		while (length > 0)
		{
			ft_putchar('0');
			--length;
		}
	}

	ft_putchar(alphabet[nb % base]);
	
}

// printing str1 then nb with base and then str2
void	ft_mal_print(const char *str1, size_t nb, size_t base, const char *str2)
{
	ft_putstr(str1);
	if (base == FT_MAL_HEX_BASE)
		ft_mal_putnbr_base(nb, base, 0, true);
	else
		ft_mal_putnbr_base(nb, base, 0, false);
	ft_putstr(str2);
}
