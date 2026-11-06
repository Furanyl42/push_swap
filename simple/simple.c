/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simple.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesmet <adesmet@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 12:53:31 by adesmet           #+#    #+#             */
/*   Updated: 2026/11/05 12:00:00 by adesmet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	ft_min_idx(t_stack *a)
{
	int	i;
	int	idx;

	i = 1;
	idx = 0;
	while (i < a->size)
	{
		if (get_stack_element(a, i) < get_stack_element(a, idx))
			idx = i;
		i++;
	}
	return (idx);
}

void	ft_min_to_top(t_stack *a)
{
	int	min;

	min = ft_min_idx(a);
	if (min > a->size / 2)
	{
		while (min < a->size)
		{
			rra(a);
			min++;
		}
	}
	else
	{
		while (min > 0)
		{
			ra(a);
			min--;
		}
	}
}

void	simple_sort(t_stack *a, t_stack *b)
{
	while (a->size > 0)
	{
		ft_min_to_top(a);
		pb(a, b);
	}
	while (b->size > 0)
		pa(a, b);
}
