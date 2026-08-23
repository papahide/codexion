/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validator.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paapahid <paapahid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 19:51:04 by paapahid          #+#    #+#             */
/*   Updated: 2026/06/17 21:33:58 by paapahid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

bool	ft_strcmp(const char *str, const char *cmp)
{
	int		i;

	i = 0;
	while(str[i])
	{
		if (str[i] != cmp[i])
			return (0);
		i++;
	}
	if (str[i] == '\0' && cmp[i] == '\0')
		return (1);
	else
		return (0);
}

bool	validator(char **str)
{
	int	i;
	int	j;

	i = -1;
	while(++i < 7)
	{
		j = -1;
		while (str[i][++j])
		{
			if(!(str[i][j] >= '0' && str[i][j] <= '9'))
				return (0);
		}
	}
	if (i == 7 && (!ft_strcmp(str[i], "edf") && !ft_strcmp(str[i], "fifo")))
		return (0);
	return (1);
}
