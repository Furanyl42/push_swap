/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_simple.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 12:00:00 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/02 12:00:00 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static int	find_min_index(t_stack *a)
{
	int	index;
	int	i;

	index = 0;
	i = 1;
	while (i < a->size)
	{
		if (get_stack_element(a, i) < get_stack_element(a, index))
			index = i;
		i++;
	}
	return (index);
}

static void	bring_min_to_top(t_stack *a)
{
	int	cost;
	int	index;

	index = find_min_index(a);
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

void	sort_simple(t_stack *a, t_stack *b)
{
	while (a->size > 0)
	{
		bring_min_to_top(a);
		pb(a, b);
	}
	while (b->size > 0)
		pa(a, b);
}
