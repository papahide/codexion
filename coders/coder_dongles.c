/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_dongles.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paapahid <paapahid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 20:16:19 by paapahid          #+#    #+#             */
/*   Updated: 2026/10/14 20:59:54 by paapahid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	coder_acquire_dongles(t_coder *coder, int coders, int coder_id)
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

void	coder_release_dongles(t_coder *coder, int coders, int coder_id)
{
	if (coders == 1)
		leave_dongle(coder, &coder->simulation->dongles[0]);
	else
	{
		leave_dongle(coder, &coder->simulation->dongles[coder_id - 1]);
		leave_dongle(coder, &coder->simulation->dongles[coder_id % coders]);
	}
}
