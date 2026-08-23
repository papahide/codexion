/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logs.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paapahid <paapahid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/23 10:05:57 by paapahid          #+#    #+#             */
/*   Updated: 2026/08/23 10:25:18 by paapahid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	print_log(t_simulation *simulation, int coder_id, char *action)
{
	pthread_mutex_lock(&simulation->log_mutex);
	printf("%ld %d %s\n", get_time_ms() - simulation->start_time, coder_id, action);
	pthread_mutex_unlock(&simulation->log_mutex);
}
