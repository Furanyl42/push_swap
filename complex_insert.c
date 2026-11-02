/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   complex_insert.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 12:00:00 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/02 12:00:00 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static void	complex_rotate_a(t_stack *a, int cost)
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

static void	complex_rotate_b(t_stack *b, int cost)
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

static void	rotate_ab(t_stack *a, t_stack *b, int cost_a, int cost_b)
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
	complex_rotate_a(a, cost_a);
	complex_rotate_b(b, cost_b);
}

void	complex_insert(t_stack *a, t_stack *b, int value)
{
	int	cost_a;
	int	cost_b;

	cost_a = get_cost(complex_target_index(a, value), a->size);
	cost_b = get_cost(complex_find(b, value), b->size);
	rotate_ab(a, b, cost_a, cost_b);
	pa(a, b);
}
