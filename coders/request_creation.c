/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request_creation.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paapahid <paapahid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/23 19:20:25 by paapahid          #+#    #+#             */
/*   Updated: 2026/08/23 19:32:34 by paapahid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_request	create_request(int coder_id, long priority)
{
	t_request	request;

	request.coder_id = coder_id;
	request.priority = priority;
	return (request);
}
