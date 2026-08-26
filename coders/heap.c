/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paapahid <paapahid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/23 17:50:25 by paapahid          #+#    #+#             */
/*   Updated: 2026/08/26 19:33:21 by paapahid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_heap	*init_heap(t_heap *heap, int max_size)
{
	heap->requests = malloc(sizeof(t_request) * max_size);
	if (!heap->requests)
		return (NULL);
	heap->size = 0;
	return (heap);
}

void	heap_push(t_heap *heap, t_request request)
{
	heap->requests[heap->size] = request;
	heap->size += 1;
}

t_request	heap_pop(t_heap *heap)
{
	t_request	to_pop;
	int			i;
	int			pos;

	i = -1;
	pos = 0;
	to_pop = heap->requests[pos];
	while (heap->size > ++i)
	{
		if (heap->requests[i].priority < to_pop.priority)
		{
			to_pop = heap->requests[i];
			pos = i;
		}
		else if ((heap->requests[i].priority == to_pop.priority)
			&& (heap->requests[i].coder_id > to_pop.coder_id))
		{
			to_pop = heap->requests[i];
			pos = i;
		}
	}
	heap->requests[pos] = heap->requests[heap->size - 1];
	heap->size -= 1;
	return (to_pop);
}
