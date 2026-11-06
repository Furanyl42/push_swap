/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ops_swap.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesmet <adesmet@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 13:49:50 by adesmet           #+#    #+#             */
/*   Updated: 2026/11/05 12:00:00 by adesmet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static void	swap_top(t_stack *s)
{
	int	temp;
	int	tidx;
	int	i1;
	int	i2;

	if (s->size < 2)
		return ;
	i1 = s->head;
	i2 = (s->head + 1) % s->capacity;
	temp = s->tab[i1];
	tidx = s->idx[i1];
	s->tab[i1] = s->tab[i2];
	s->tab[i2] = temp;
	s->idx[i1] = s->idx[i2];
	s->idx[i2] = tidx;
}

void	sa(t_stack *a)
{
	if (a->size < 2)
		return ;
	swap_top(a);
	a->count->sa++;
	write(1, "sa\n", 3);
}

void	sb(t_stack *b)
{
	if (b->size < 2)
		return ;
	swap_top(b);
	b->count->sb++;
	write(1, "sb\n", 3);
}

void	ss(t_stack *a, t_stack *b)
{
	swap_top(a);
	swap_top(b);
	a->count->ss++;
	write(1, "ss\n", 3);
}
