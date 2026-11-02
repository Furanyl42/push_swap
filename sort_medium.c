/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_medium.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 12:00:00 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/02 12:00:00 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static int	floor_sqrt(int n)
{
	int	root;

	root = 0;
	while ((root + 1) <= n / (root + 1))
		root++;
	return (root);
}

static void	medium_chunks(t_stack *a, t_stack *b, int div)
{
	int	size;
	int	chunk;
	int	count;
	int	i;

	size = a->size / div;
	if (size < 1)
		size = 1;
	chunk = 1;
	while (a->size > 0)
	{
		count = a->size;
		i = 0;
		while (i < count)
		{
			if (get_stack_element(a, 0) < chunk * size)
				pb(a, b);
			else
				ra(a);
			i++;
		}
		chunk++;
	}
}

void	sort_medium(t_stack *a, t_stack *b)
{
	t_geom	geom;

	if (a->size < 2)
		return ;
	geom.n = a->size;
	if (!normalize_stack(a))
	{
		sort_simple(a, b);
		return ;
	}
	geom.div = floor_sqrt(geom.n) / 4;
	if (geom.div < 2)
		geom.div = 2;
	geom.fr = 1;
	medium_chunks(a, b, geom.div);
	complex_pull(a, b, &geom);
}
