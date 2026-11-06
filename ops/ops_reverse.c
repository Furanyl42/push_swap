/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ops_reverse.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesmet <adesmet@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 13:49:40 by adesmet           #+#    #+#             */
/*   Updated: 2026/11/05 12:00:00 by adesmet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static void	reverse_top(t_stack *s)
{
	int	val;
	int	idx;
	int	new_head;

	val = s->tab[(s->head + s->size - 1) % s->capacity];
	idx = s->idx[(s->head + s->size - 1) % s->capacity];
	new_head = (s->head - 1 + s->capacity) % s->capacity;
	s->tab[new_head] = val;
	s->idx[new_head] = idx;
	s->head = new_head;
}

void	rra(t_stack *a)
{
	if (a->size < 2)
		return ;
	reverse_top(a);
	a->count->rra++;
	write(1, "rra\n", 4);
}

void	rrb(t_stack *b)
{
	if (b->size < 2)
		return ;
	reverse_top(b);
	b->count->rrb++;
	write(1, "rrb\n", 4);
}

void	rrr(t_stack *a, t_stack *b)
{
	if (a->size >= 2)
		reverse_top(a);
	if (b->size >= 2)
		reverse_top(b);
	a->count->rrr++;
	write(1, "rrr\n", 4);
}
