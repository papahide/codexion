/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paapahid <paapahid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 20:33:50 by paapahid          #+#    #+#             */
/*   Updated: 2026/08/26 20:35:47 by paapahid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	coder_acquire_dongles(t_coder *coder, int coders, int coder_id)
{
	if (coders == 1)
	{
		take_dongle(coder, &coder->simulation->dongles[0]);
		print_log(coder->simulation, coder->id, "has taken a dongle");
	}
	else
		take_both_dongles(coder, &coder->simulation->dongles[coder_id - 1],
			&coder->simulation->dongles[coder_id % coders]);
}

static void	coder_release_dongles(t_coder *coder, int coders, int coder_id)
{
	if (coders == 1)
		leave_dongle(coder, &coder->simulation->dongles[0]);
	else
	{
		leave_dongle(coder, &coder->simulation->dongles[coder_id - 1]);
		leave_dongle(coder, &coder->simulation->dongles[coder_id % coders]);
	}
}

static int	coder_do_phase(t_coder *coder, char *message, int sleep_ms)
{
	print_log(coder->simulation, coder->id, message);
	usleep(sleep_ms * 1000);
	return (coder->simulation->stop);
}

int	coder_cycle(t_coder *coder, int coders, int coder_id, t_parameters param)
{
	coder_acquire_dongles(coder, coders, coder_id);
	if (coder->simulation->stop)
		return (1);
	coder->last_compile_start = get_time_ms();
	if (coder_do_phase(coder, "is compiling", param.time_to_compile))
		return (1);
	coder->compiles_done += 1;
	coder_release_dongles(coder, coders, coder_id);
	if (coder->simulation->stop)
		return (1);
	if (coder_do_phase(coder, "is debugging", param.time_to_debug))
		return (1);
	return (coder_do_phase(coder, "is refactoring", param.time_to_refactor));
}
