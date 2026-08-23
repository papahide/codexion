/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paapahid <paapahid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 22:43:55 by paapahid          #+#    #+#             */
/*   Updated: 2026/08/20 21:40:38 by paapahid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdio.h>

void	codexion(t_parameters *params)
{
	t_simulation *sim;

	sim = init_simulation(params);
	start_simulation(sim);
}

int main(int argc, char *argv[])
{
	char			**args;
	t_parameters	*valid;

	if (argc != 9)
	{
		write(2, "Incorrect number of arguments.\n", 31);
		return (1);
	}
	args = &argv[1];
	if (!validator(args))
	{
		write(2, "Invalid arguments.\n", 19);
		return (1);
	}
	valid = malloc(sizeof(t_parameters));
	if (!valid)
		return (1);
	parse(valid, args);
	codexion(valid);
	free(valid);
	return (0);
}
