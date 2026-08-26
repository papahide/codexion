/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paapahid <paapahid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/23 09:58:06 by paapahid          #+#    #+#             */
/*   Updated: 2026/08/26 20:39:28 by paapahid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void	leave_dongle(t_coder *coder, t_dongle *dongle)
{
	int	cooldown;

	cooldown = coder->simulation->parameters.dongle_cooldown;
	pthread_mutex_lock(&dongle->mutex);
	dongle->available = 1;
	dongle->available_at = get_time_ms() + cooldown;
	pthread_mutex_unlock(&dongle->mutex);
}

int	check_compiles_done(t_simulation *sim)
{
	int	compiles_done;
	int	i;

	i = -1;
	compiles_done = 1;
	while (sim->parameters.number_of_coders > ++i)
	{
		if (sim->coders[i].compiles_done < sim->parameters.comp_required)
		{
			compiles_done = 0;
			break ;
		}
	}
	return (compiles_done);
}

int	set_sim_stop(t_simulation *sim)
{
	int	stop;

	pthread_mutex_lock(&sim->stop_mutex);
	stop = sim->stop;
	pthread_mutex_unlock(&sim->stop_mutex);
	return (stop);
}
