/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   take_dongle.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paapahid <paapahid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 20:38:52 by paapahid          #+#    #+#             */
/*   Updated: 2026/08/26 20:47:33 by paapahid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static t_request	make_request(t_coder *coder)
{
	if (coder->simulation->parameters.scheduler == 0)
		return (create_request(coder->id, get_time_ms()));
	return (create_request(coder->id, coder->last_compile_start
			+ coder->simulation->parameters.time_to_burnout));
}

static void	wait_for_dongle(t_coder *coder, t_dongle *dongle)
{
	while (!coder->simulation->stop
		&& (dongle->queue.requests[0].coder_id != coder->id
			|| dongle->available == 0
			|| get_time_ms() < dongle->available_at))
	{
		pthread_mutex_unlock(&dongle->mutex);
		usleep(500);
		pthread_mutex_lock(&dongle->mutex);
	}
}

void	take_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	heap_push(&dongle->queue, make_request(coder));
	wait_for_dongle(coder, dongle);
	heap_pop(&dongle->queue);
	if (coder->simulation->stop)
	{
		pthread_mutex_unlock(&dongle->mutex);
		return ;
	}
	dongle->available = 0;
	pthread_mutex_unlock(&dongle->mutex);
}
