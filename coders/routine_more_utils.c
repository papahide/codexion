/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine_more_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paapahid <paapahid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 20:27:28 by paapahid          #+#    #+#             */
/*   Updated: 2026/10/14 21:00:22 by paapahid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	get_compile_start(t_coder *coder)
{
	long	last_compile;

	pthread_mutex_lock(&coder->mutex);
	last_compile = coder->last_compile_start;
	pthread_mutex_unlock(&coder->mutex);
	return (last_compile);
}

int	get_compiles_done(t_coder *coder)
{
	int	compiles;

	pthread_mutex_lock(&coder->mutex);
	compiles = coder->compiles_done;
	pthread_mutex_unlock(&coder->mutex);
	return (compiles);
}
