/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_mal_precomp.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aashara- <aashara-@student.21-school.ru    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/20 10:34:09 by aashara-          #+#    #+#             */
/*   Updated: 2021/10/20 10:34:09 by aashara-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_MAL_PRECOMP_H
# define FT_MAL_PRECOMP_H

/*
******************************** System headers ********************************
*/

/*
************* Boolean type and values ************
*/
# include <stdbool.h>

/*
****************** pthread_mutex_t ********
****************** pthread_mutex_trylock **
****************** pthread_mutex_lock *****
****************** pthread_mutex_unlock ***
****************** pthread_mutex_init *****
****************** pthread_mutex_destroy **
*/
# include <pthread.h>


/*
*************************** User headers ************************************
*/

/*
******************* ft_bzero **************
*/
#include "libft.h"


/*
******************************** Macroses **************************************
*/

/*
************* Return status macroses *************
*/
# define FT_MAL_SUCCESS 0
# define FT_MAL_FAILURE -1

/*
******************* Mutex operations **********************
*/
# define FT_MAL_MUTEX_TRYLOCK(mutex_ptr) (pthread_mutex_trylock(mutex_ptr))
# define FT_MAL_MUTEX_LOCK(mutex_ptr) (pthread_mutex_lock(mutex_ptr))
# define FT_MAL_MUTEX_UNLOCK(mutex_ptr) (pthread_mutex_unlock(mutex_ptr))


#endif
