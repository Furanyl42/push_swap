/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ops_rotate.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesmet <adesmet@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 13:49:46 by adesmet           #+#    #+#             */
/*   Updated: 2026/11/05 12:00:00 by adesmet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static void	rotate_top(t_stack *s)
{
	int	val;
	int	idx;
	int	tail;

	val = s->tab[s->head];
	idx = s->idx[s->head];
	tail = (s->head + s->size) % s->capacity;
	s->tab[tail] = val;
	s->idx[tail] = idx;
	s->head = (s->head + 1) % s->capacity;
}

void	ra(t_stack *a)
{
	if (a->size < 2)
		return ;
	rotate_top(a);
	a->count->ra++;
	write(1, "ra\n", 3);
}

void	rb(t_stack *b)
{
	if (b->size < 2)
		return ;
	rotate_top(b);
	b->count->rb++;
	write(1, "rb\n", 3);
}

void	rr(t_stack *a, t_stack *b)
{
	if (a->size >= 2)
		rotate_top(a);
	if (b->size >= 2)
		rotate_top(b);
	a->count->rr++;
	write(1, "rr\n", 3);
}
