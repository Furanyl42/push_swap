/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_complex.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 12:00:00 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/02 12:00:00 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static void	complex_geometry(int n, int *div, int *fr)
{
	if (n < 300)
	{
		*div = 2;
		*fr = 3;
	}
	else if (n < 450)
	{
		*div = 3;
		*fr = 3;
	}
	else if (n < 800)
	{
		*div = 4;
		*fr = 3;
	}
	else
	{
		*div = 4;
		*fr = 4;
	}
}

void	sort_complex(t_stack *a, t_stack *b)
{
	t_geom	geom;

	geom.n = a->size;
	if (geom.n < 2)
		return ;
	if (!normalize_stack(a))
	{
		sort_simple(a, b);
		return ;
	}
	complex_geometry(geom.n, &geom.div, &geom.fr);
	complex_presort(a, b, geom.div, geom.fr);
	complex_pull(a, b, &geom);
}
