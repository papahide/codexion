/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initialization.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paapahid <paapahid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 20:10:02 by paapahid          #+#    #+#             */
/*   Updated: 2026/08/26 19:32:34 by paapahid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	init_dongles(int dongle_n, t_simulation *simulation)
{
	int	i;

	i = -1;
	while (dongle_n > ++i)
	{
		pthread_mutex_init(&simulation->dongles[i].mutex, NULL);
		pthread_cond_init(&simulation->dongles[i].cond, NULL);
		init_heap(&simulation->dongles[i].queue, dongle_n);
		simulation->dongles[i].available_at = 0;
		simulation->dongles[i].available = 1;
	}
}

void	init_coders(int coders_n, t_simulation *simulation)
{
	int	i;

	i = -1;
	while (coders_n > ++i)
	{
		simulation->coders[i].id = i + 1;
		simulation->coders[i].compiles_done = 0;
		simulation->coders[i].last_compile_start = get_time_ms();
		simulation->coders[i].simulation = simulation;
	}
}
