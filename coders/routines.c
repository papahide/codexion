/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routines.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paapahid <paapahid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 21:32:57 by paapahid          #+#    #+#             */
/*   Updated: 2026/08/26 20:48:23 by paapahid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	take_both_dongles(t_coder *coder, t_dongle *left, t_dongle *right)
{
	if (coder->id % 2 == 0)
	{
		take_dongle(coder, right);
		if (coder->simulation->stop)
		{
			leave_dongle(coder, right);
			return ;
		}
		print_log(coder->simulation, coder->id, "has taken a dongle");
		take_dongle(coder, left);
		print_log(coder->simulation, coder->id, "has taken a dongle");
	}
	else
	{
		take_dongle(coder, left);
		if (coder->simulation->stop)
		{
			leave_dongle(coder, left);
			return ;
		}
		print_log(coder->simulation, coder->id, "has taken a dongle");
		take_dongle(coder, right);
		print_log(coder->simulation, coder->id, "has taken a dongle");
	}
}

void	*coder_routine(void *arg)
{
	t_parameters	param;
	t_coder			*coder;
	int				coders;

	coder = (t_coder *)arg;
	coders = coder->simulation->parameters.number_of_coders;
	param = coder->simulation->parameters;
	while (!coder->simulation->stop)
	{
		if (coder_cycle(coder, coders, coder->id, param))
			break ;
	}
	return (NULL);
}

void	*monitor_routine(void *arg)
{
	t_simulation	*sim;
	int				i;
	long			coder_deadline;
	int				stop;

	i = -1;
	sim = (t_simulation *)arg;
	stop = 0;
	while (stop == 0)
	{
		i = -1;
		while (sim->parameters.number_of_coders > ++i)
		{
			coder_deadline = sim->coders[i].last_compile_start
				+ sim->parameters.time_to_burnout;
			if (get_time_ms() > coder_deadline)
				stop_simulation(sim, sim->coders[i].id, 1);
		}
		if (check_compiles_done(sim) == 1)
			stop_simulation(sim, 0, 0);
		usleep(1000);
		stop = set_sim_stop(sim);
	}
	return (NULL);
}
