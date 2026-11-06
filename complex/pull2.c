/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pull2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/04 12:23:40 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/05 12:00:00 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "complex.h"
#include "push_swap.h"

int	best_in_range(t_stack *a, t_stack *b, t_range *range, int *best_val)
{
	int	i;
	int	best;
	int	value;
	int	score;
	int	found;

	best = INT_MAX;
	found = 0;
	i = 0;
	while (i < b->size)
	{
		value = b->idx[(b->head + i) % b->capacity];
		if (value >= range->low && value < range->high)
		{
			score = score_at(a, b, i, value);
			if (score < best)
			{
				best = score;
				*best_val = value;
				found = 1;
			}
		}
		i++;
	}
	return (found);
}

static void	pull_range(t_stack *a, t_stack *b, t_range *range)
{
	int	value;

	while (best_in_range(a, b, range, &value))
		complex_insert(a, b, value);
}

void	complex_pull(t_stack *a, t_stack *b, t_geom *geom)
{
	int		chunk_idx;
	int		base;
	int		mid;
	t_range	range;

	chunk_idx = geom->div;
	while (chunk_idx >= 0)
	{
		base = chunk_idx * geom->chunk_size;
		mid = base + geom->chunk_size / geom->fr;
		range.low = mid;
		range.high = base + geom->chunk_size;
		pull_range(a, b, &range);
		range.low = base;
		range.high = mid;
		pull_range(a, b, &range);
		chunk_idx--;
	}
}

void	complex_align(t_stack *a)
{
	int	min_idx;
	int	cost;

	min_idx = get_min_idx(a);
	cost = get_cost(min_idx, a->size);
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

void	complex_sort(t_stack *a, t_stack *b)
{
	t_geom	geom;

	if (a->size < 2)
		return ;
	geom.n = a->size;
	complex_geometry(&geom);
	geom.chunk_size = geom.n / geom.div;
	if (geom.chunk_size < 1)
		geom.chunk_size = 1;
	index_stack(a);
	complex_presort(a, b, &geom);
	complex_pull(a, b, &geom);
	complex_align(a);
}
