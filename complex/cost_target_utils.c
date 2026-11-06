/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cost_target_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/04 11:08:44 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/05 12:00:00 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "complex.h"
#include "push_swap.h"

int	get_cost(int index, int size)
{
	if (size <= 1)
		return (0);
	if (index <= size / 2)
		return (index);
	return (index - size);
}

int	get_min_idx(t_stack *a)
{
	int	i;
	int	min_val;
	int	min_idx;
	int	current;

	if (a->size == 0)
		return (0);
	min_idx = 0;
	min_val = a->idx[a->head];
	i = 1;
	while (i < a->size)
	{
		current = a->idx[(a->head + i) % a->capacity];
		if (current < min_val)
		{
			min_val = current;
			min_idx = i;
		}
		i++;
	}
	return (min_idx);
}

int	get_target_idx(t_stack *a, int value)
{
	int	i;
	int	best_idx;
	int	best_val;
	int	current;

	best_idx = -1;
	best_val = INT_MAX;
	i = 0;
	while (i < a->size)
	{
		current = a->idx[(a->head + i) % a->capacity];
		if (current > value && current < best_val)
		{
			best_val = current;
			best_idx = i;
		}
		i++;
	}
	if (best_idx == -1)
		best_idx = get_min_idx(a);
	return (best_idx);
}

int	combined_cost(int cost_a, int cost_b)
{
	int	abs_a;
	int	abs_b;

	abs_a = cost_a;
	if (abs_a < 0)
		abs_a = -abs_a;
	abs_b = cost_b;
	if (abs_b < 0)
		abs_b = -abs_b;
	if ((cost_a > 0 && cost_b > 0) || (cost_a < 0 && cost_b < 0))
	{
		if (abs_a > abs_b)
			return (abs_a);
		return (abs_b);
	}
	return (abs_a + abs_b);
}

int	score_at(t_stack *a, t_stack *b, int i, int value)
{
	int	target;
	int	cost_a;
	int	cost_b;

	target = get_target_idx(a, value);
	cost_a = get_cost(target, a->size);
	cost_b = get_cost(i, b->size);
	return (combined_cost(cost_a, cost_b));
}
