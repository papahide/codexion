/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paapahid <paapahid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 20:33:50 by paapahid          #+#    #+#             */
/*   Updated: 2026/10/14 20:17:39 by paapahid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	coder_do_phase(t_coder *coder, char *message, int sleep_ms)
{
	print_log(coder->simulation, coder->id, message);
	usleep(sleep_ms * 1000);
	return (set_sim_stop(coder->simulation));
}

static void	set_compile_start(t_coder *coder)
{
	pthread_mutex_lock(&coder->mutex);
	coder->last_compile_start = get_time_ms();
	pthread_mutex_unlock(&coder->mutex);
}

static void	add_compile_done(t_coder *coder)
{
	pthread_mutex_lock(&coder->mutex);
	coder->compiles_done += 1;
	pthread_mutex_unlock(&coder->mutex);
}

int	coder_cycle(t_coder *coder, int coders, int coder_id, t_parameters param)
{
	coder_acquire_dongles(coder, coders, coder_id);
	if (set_sim_stop(coder->simulation))
		return (1);
	set_compile_start(coder);
	if (coder_do_phase(coder, "is compiling", param.time_to_compile))
		return (1);
	add_compile_done(coder);
	coder_release_dongles(coder, coders, coder_id);
	if (set_sim_stop(coder->simulation))
		return (1);
	if (coder_do_phase(coder, "is debugging", param.time_to_debug))
		return (1);
	return (coder_do_phase(coder, "is refactoring", param.time_to_refactor));
}
