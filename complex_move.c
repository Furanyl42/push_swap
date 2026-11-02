/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   complex_move.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 12:00:00 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/02 12:00:00 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	complex_find(t_stack *s, int value)
{
	int	i;

	i = 0;
	while (i < s->size)
	{
		if (get_stack_element(s, i) == value)
			return (i);
		i++;
	}
	return (0);
}

int	complex_min_index(t_stack *a)
{
	int	idx;
	int	i;

	idx = 0;
	i = 1;
	while (i < a->size)
	{
		if (get_stack_element(a, i) < get_stack_element(a, idx))
			idx = i;
		i++;
	}
	return (idx);
}

static int	best_greater(t_stack *a, int value)
{
	int	idx;
	int	best;
	int	i;

	idx = -1;
	best = 2147483647;
	i = 0;
	while (i < a->size)
	{
		if (get_stack_element(a, i) > value
			&& get_stack_element(a, i) < best)
		{
			best = get_stack_element(a, i);
			idx = i;
		}
		i++;
	}
	return (idx);
}

int	complex_target_index(t_stack *a, int value)
{
	int	idx;

	idx = best_greater(a, value);
	if (idx == -1)
		idx = complex_min_index(a);
	return (idx);
}

void	complex_align(t_stack *a)
{
	int	index;
	int	cost;

	index = complex_min_index(a);
	cost = get_cost(index, a->size);
	while (cost > 0)
	{
		ra(a);
		cost--;
	}
	while (cost < 0)
	{
		rra(a);
		cost++;
	}
}
