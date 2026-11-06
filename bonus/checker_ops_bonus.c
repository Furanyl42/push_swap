/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checker_ops_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesmet <adesmet@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/05 13:00:00 by adesmet           #+#    #+#             */
/*   Updated: 2026/11/05 13:00:00 by adesmet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "checker_bonus.h"

void	chk_swap(t_stack *s)
{
	int	tmp;
	int	tidx;
	int	i1;
	int	i2;

	if (s->size < 2)
		return ;
	i1 = s->head;
	i2 = (s->head + 1) % s->capacity;
	tmp = s->tab[i1];
	tidx = s->idx[i1];
	s->tab[i1] = s->tab[i2];
	s->tab[i2] = tmp;
	s->idx[i1] = s->idx[i2];
	s->idx[i2] = tidx;
}

void	chk_rotate(t_stack *s)
{
	int	val;
	int	idx;
	int	tail;

	if (s->size < 2)
		return ;
	val = s->tab[s->head];
	idx = s->idx[s->head];
	tail = (s->head + s->size) % s->capacity;
	s->tab[tail] = val;
	s->idx[tail] = idx;
	s->head = (s->head + 1) % s->capacity;
}

void	chk_reverse(t_stack *s)
{
	int	val;
	int	idx;
	int	last;
	int	new_head;

	if (s->size < 2)
		return ;
	last = (s->head + s->size - 1) % s->capacity;
	val = s->tab[last];
	idx = s->idx[last];
	new_head = (s->head - 1 + s->capacity) % s->capacity;
	s->tab[new_head] = val;
	s->idx[new_head] = idx;
	s->head = new_head;
}

void	chk_push(t_stack *dst, t_stack *src)
{
	int	val;
	int	idx;

	if (src->size == 0)
		return ;
	val = src->tab[src->head];
	idx = src->idx[src->head];
	src->head = (src->head + 1) % src->capacity;
	src->size--;
	dst->head = (dst->head - 1 + dst->capacity) % dst->capacity;
	dst->tab[dst->head] = val;
	dst->idx[dst->head] = idx;
	dst->size++;
}

int	chk_streq(const char *a, const char *b)
{
	while (*a && *a == *b)
	{
		a++;
		b++;
	}
	return (*a == *b);
}
