/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pull1.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/04 12:09:15 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/05 12:00:00 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "complex.h"
#include "push_swap.h"

int	complex_find(t_stack *b, int value)
{
	int	i;

	i = 0;
	while (i < b->size)
	{
		if (b->idx[(b->head + i) % b->capacity] == value)
			return (i);
		i++;
	}
	return (-1);
}

static void	rotate_a(t_stack *a, int cost)
{
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

static void	rotate_b(t_stack *b, int cost)
{
	while (cost > 0)
	{
		rb(b);
		cost--;
	}
	while (cost < 0)
	{
		rrb(b);
		cost++;
	}
}

void	rotate_ab(t_stack *a, t_stack *b, int cost_a, int cost_b)
{
	while (cost_a > 0 && cost_b > 0)
	{
		rr(a, b);
		cost_a--;
		cost_b--;
	}
	while (cost_a < 0 && cost_b < 0)
	{
		rrr(a, b);
		cost_a++;
		cost_b++;
	}
	rotate_a(a, cost_a);
	rotate_b(b, cost_b);
}

void	complex_insert(t_stack *a, t_stack *b, int value)
{
	int	target;
	int	b_idx;
	int	cost_a;
	int	cost_b;

	target = get_target_idx(a, value);
	b_idx = complex_find(b, value);
	cost_a = get_cost(target, a->size);
	cost_b = get_cost(b_idx, b->size);
	rotate_ab(a, b, cost_a, cost_b);
	pa(a, b);
}
