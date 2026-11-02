/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   complex_pull.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 12:00:00 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/02 12:00:00 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	complex_cost(int cost_a, int cost_b)
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

static int	candidate_score(t_stack *a, t_stack *b, int value, int index)
{
	int	cost_a;
	int	cost_b;

	cost_a = get_cost(complex_target_index(a, value), a->size);
	cost_b = get_cost(index, b->size);
	return (complex_cost(cost_a, cost_b));
}

static int	best_in_range(t_stack *a, t_stack *b, t_range *range, int *out)
{
	int	i;
	int	best;
	int	score;
	int	value;

	i = 0;
	best = 2147483647;
	*out = 0;
	while (i < b->size)
	{
		value = get_stack_element(b, i);
		if (value >= range->low && value < range->high)
		{
			score = candidate_score(a, b, value, i);
			if (score < best)
			{
				best = score;
				*out = value;
			}
		}
		i++;
	}
	return (best != 2147483647);
}

static void	pull_range(t_stack *a, t_stack *b, t_range *range)
{
	int	value;

	while (best_in_range(a, b, range, &value))
		complex_insert(a, b, value);
}

void	complex_pull(t_stack *a, t_stack *b, t_geom *geom)
{
	int		chunk_size;
	int		chunk;
	int		base;
	int		mid;
	t_range	range;

	chunk_size = geom->n / geom->div;
	if (chunk_size < 1)
		chunk_size = 1;
	chunk = geom->div;
	while (chunk >= 0)
	{
		base = chunk * chunk_size;
		mid = base + chunk_size / geom->fr;
		range.low = mid;
		range.high = base + chunk_size;
		pull_range(a, b, &range);
		range.low = base;
		range.high = mid;
		pull_range(a, b, &range);
		chunk--;
	}
	complex_align(a);
}
