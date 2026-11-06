/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ops_push.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesmet <adesmet@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 13:48:58 by adesmet           #+#    #+#             */
/*   Updated: 2026/11/05 12:00:00 by adesmet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	pb(t_stack *a, t_stack *b)
{
	int	val;
	int	idx;

	if (a->size == 0)
		return ;
	val = a->tab[a->head];
	idx = a->idx[a->head];
	a->head = (a->head + 1) % a->capacity;
	a->size--;
	b->head = (b->head - 1 + b->capacity) % b->capacity;
	b->tab[b->head] = val;
	b->idx[b->head] = idx;
	b->size++;
	a->count->pb++;
	write(1, "pb\n", 3);
}

void	pa(t_stack *a, t_stack *b)
{
	int	val;
	int	idx;

	if (b->size == 0)
		return ;
	val = b->tab[b->head];
	idx = b->idx[b->head];
	b->head = (b->head + 1) % b->capacity;
	b->size--;
	a->head = (a->head - 1 + a->capacity) % a->capacity;
	a->tab[a->head] = val;
	a->idx[a->head] = idx;
	a->size++;
	a->count->pa++;
	write(1, "pa\n", 3);
}
