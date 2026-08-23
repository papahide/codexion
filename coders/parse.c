/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paapahid <paapahid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 22:44:01 by paapahid          #+#    #+#             */
/*   Updated: 2026/08/23 16:04:55 by paapahid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static long	ft_atol(const char *nbr)
{
	long	n;
	int		i;

	i = -1;
	n = 0;
	while (nbr[++i])
	{
		n = n * 10 + (nbr[i] - '0');
	}
	return (n);
}

void	parse(t_parameters *valid, char **args)
{
	valid->number_of_coders = ft_atol(args[0]);
	valid->time_to_burnout = ft_atol(args[1]);
	valid->time_to_compile = ft_atol(args[2]);
	valid->time_to_debug = ft_atol(args[3]);
	valid->time_to_refactor = ft_atol(args[4]);
	valid->comp_required = ft_atol(args[5]);
	valid->dongle_cooldown = ft_atol(args[6]);
	if (ft_strcmp(args[7], "fifo"))
		valid->scheduler = 0;
	else
		valid->scheduler = 1;
}
