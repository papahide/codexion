/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routines.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paapahid <paapahid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 21:32:57 by paapahid          #+#    #+#             */
/*   Updated: 2026/08/25 21:14:16 by paapahid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	take_both_dongles(t_coder *coder, t_dongle *left, t_dongle *right)
{
	if (coder->id % 2 == 0)
	{
		take_dongle(coder, right);
		if (coder->simulation->stop)
		{
			leave_dongle(coder, right);
			return;
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
			return;
		}
		print_log(coder->simulation, coder->id, "has taken a dongle");
		take_dongle(coder, right);
		print_log(coder->simulation, coder->id, "has taken a dongle");
	}
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;
	int		coder_id;
	int		coders;

	coder = (t_coder *)arg;
	coder_id = coder->id;
	coders = coder->simulation->parameters.number_of_coders;
	while (!coder->simulation->stop)
	{
		if (coders == 1)
		{
			take_dongle(coder, &coder->simulation->dongles[0]);
			print_log(coder->simulation, coder->id, "has taken a dongle");
		}
		else
			take_both_dongles(coder, &coder->simulation->dongles[coder_id - 1],
				&coder->simulation->dongles[coder_id % coders]);
		if (coder->simulation->stop)
			break;
		coder->last_compile_start = get_time_ms();
		print_log(coder->simulation, coder_id, "is compiling");
		if (coder->simulation->stop)
			break;
		usleep(coder->simulation->parameters.time_to_compile * 1000);
		coder->compiles_done += 1;
		if (coders == 1)
			leave_dongle(coder, &coder->simulation->dongles[0]);
		else
		{
			leave_dongle(coder, &coder->simulation->dongles[coder_id - 1]);
			leave_dongle(coder, &coder->simulation->dongles[coder_id % coders]);
		}
		if (coder->simulation->stop)
			break;
		print_log(coder->simulation, coder_id, "is debugging");
		usleep(coder->simulation->parameters.time_to_debug * 1000);
		if (coder->simulation->stop)
			break;
		print_log(coder->simulation, coder_id, "is refactoring");
		usleep(coder->simulation->parameters.time_to_refactor * 1000);
		if (coder->simulation->stop)
			break;
	}
	return NULL;
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
	return NULL;
}
