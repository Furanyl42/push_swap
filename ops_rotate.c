/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ops_rotate.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 12:00:00 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/02 12:00:00 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	ra(t_stack *a)
{
	int	value;
	int	tail;

	if (a->size < 2)
		return ;
	value = a->tab[a->head];
	tail = (a->head + a->size) % a->capacity;
	a->tab[tail] = value;
	a->head = (a->head + 1) % a->capacity;
	a->counts->ra++;
	if (!a->quiet)
		write(1, "ra\n", 3);
}

void	rb(t_stack *b)
{
	int	value;
	int	tail;

	if (b->size < 2)
		return ;
	value = b->tab[b->head];
	tail = (b->head + b->size) % b->capacity;
	b->tab[tail] = value;
	b->head = (b->head + 1) % b->capacity;
	b->counts->rb++;
	if (!b->quiet)
		write(1, "rb\n", 3);
}

void	rr(t_stack *a, t_stack *b)
{
	int	value;
	int	tail;

	if (a->size < 2 && b->size < 2)
		return ;
	if (a->size >= 2)
	{
		value = a->tab[a->head];
		tail = (a->head + a->size) % a->capacity;
		a->tab[tail] = value;
		a->head = (a->head + 1) % a->capacity;
	}
	if (b->size >= 2)
	{
		value = b->tab[b->head];
		tail = (b->head + b->size) % b->capacity;
		b->tab[tail] = value;
		b->head = (b->head + 1) % b->capacity;
	}
	a->counts->rr++;
	if (!a->quiet)
		write(1, "rr\n", 3);
}
