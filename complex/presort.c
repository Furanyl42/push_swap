/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   presort.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/04 12:21:13 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/05 12:00:00 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "complex.h"
#include "push_swap.h"

void	complex_geometry(t_geom *geom)
{
	if (geom->n < 300)
	{
		geom->div = 2;
		geom->fr = 3;
	}
	else if (geom->n < 450)
	{
		geom->div = 3;
		geom->fr = 3;
	}
	else if (geom->n < 800)
	{
		geom->div = 4;
		geom->fr = 3;
	}
	else
	{
		geom->div = 4;
		geom->fr = 4;
	}
}

static void	presort_one(t_stack *a, t_stack *b, t_geom *geom, int chunk_idx)
{
	int	rank_a;
	int	rank_b;

	rank_a = a->idx[a->head];
	if (rank_a < (chunk_idx + 1) * geom->chunk_size)
	{
		pb(a, b);
		rank_b = b->idx[b->head];
		if (rank_b < chunk_idx * geom->chunk_size
			+ geom->chunk_size / geom->fr)
			rb(b);
	}
	else
		ra(a);
}

void	complex_presort(t_stack *a, t_stack *b, t_geom *geom)
{
	int	chunk_idx;
	int	count;
	int	i;

	chunk_idx = 0;
	while (a->size > 0)
	{
		count = a->size;
		i = 0;
		while (i < count)
		{
			presort_one(a, b, geom, chunk_idx);
			i++;
		}
		chunk_idx++;
	}
}
