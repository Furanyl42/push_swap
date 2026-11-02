/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ops_reverse.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 12:00:00 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/02 12:00:00 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	rra(t_stack *a)
{
	int	value;
	int	new_head;

	if (a->size < 2)
		return ;
	value = a->tab[(a->head + a->size - 1) % a->capacity];
	new_head = (a->head - 1 + a->capacity) % a->capacity;
	a->tab[new_head] = value;
	a->head = new_head;
	a->counts->rra++;
	if (!a->quiet)
		write(1, "rra\n", 4);
}

void	rrb(t_stack *b)
{
	int	value;
	int	new_head;

	if (b->size < 2)
		return ;
	value = b->tab[(b->head + b->size - 1) % b->capacity];
	new_head = (b->head - 1 + b->capacity) % b->capacity;
	b->tab[new_head] = value;
	b->head = new_head;
	b->counts->rrb++;
	if (!b->quiet)
		write(1, "rrb\n", 4);
}

void	rrr(t_stack *a, t_stack *b)
{
	int	value;
	int	new_head;

	if (a->size < 2 && b->size < 2)
		return ;
	if (a->size >= 2)
	{
		value = a->tab[(a->head + a->size - 1) % a->capacity];
		new_head = (a->head - 1 + a->capacity) % a->capacity;
		a->tab[new_head] = value;
		a->head = new_head;
	}
	if (b->size >= 2)
	{
		value = b->tab[(b->head + b->size - 1) % b->capacity];
		new_head = (b->head - 1 + b->capacity) % b->capacity;
		b->tab[new_head] = value;
		b->head = new_head;
	}
	a->counts->rrr++;
	if (!a->quiet)
		write(1, "rrr\n", 4);
}
