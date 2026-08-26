/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paapahid <paapahid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/23 16:07:44 by paapahid          #+#    #+#             */
/*   Updated: 2026/08/26 19:35:17 by paapahid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_simulation	*init_simulation(t_parameters *params)
{
	t_simulation	*simulation;

	simulation = malloc(sizeof(t_simulation));
	if (!simulation)
		return (NULL);
	simulation->parameters = *params;
	simulation->dongles = malloc(sizeof(t_dongle) * params->number_of_coders);
	if (!simulation->dongles)
		return (free(simulation), NULL);
	simulation->coders = malloc(sizeof(t_coder) * params->number_of_coders);
	if (!simulation->coders)
		return (free(simulation->dongles), free(simulation), NULL);
	simulation->stop = 0;
	simulation->start_time = get_time_ms();
	pthread_mutex_init(&simulation->log_mutex, NULL);
	pthread_mutex_init(&simulation->stop_mutex, NULL);
	init_coders(params->number_of_coders, simulation);
	init_dongles(params->number_of_coders, simulation);
	return (simulation);
}

static void	free_simulation(t_simulation *sim)
{
	int	coder_nr;
	int	i;

	i = -1;
	coder_nr = sim->parameters.number_of_coders;
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->stop_mutex);
	while (coder_nr > ++i)
	{
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		pthread_cond_destroy(&sim->dongles[i].cond);
		free(sim->dongles[i].queue.requests);
	}
	free(sim->dongles);
	free(sim->coders);
	free(sim);
}

void	start_simulation(t_simulation *sim)
{
	int	i;

	i = -1;
	while (sim->parameters.number_of_coders > ++i)
		pthread_create(&sim->coders[i].thread, NULL,
			coder_routine, &sim->coders[i]);
	pthread_create(&sim->monitor, NULL, monitor_routine, sim);
	i = -1;
	while (sim->parameters.number_of_coders > ++i)
		pthread_join(sim->coders[i].thread, NULL);
	pthread_join(sim->monitor, NULL);
	free_simulation(sim);
}

void	stop_simulation(t_simulation *sim, int coder_id, int reason)
{
	if (reason)
	{
		pthread_mutex_lock(&sim->stop_mutex);
		print_log(sim, coder_id, "burned out");
		sim->stop = 1;
		pthread_mutex_unlock(&sim->stop_mutex);
	}
	else
	{
		pthread_mutex_lock(&sim->stop_mutex);
		sim->stop = 1;
		pthread_mutex_unlock(&sim->stop_mutex);
	}
}
