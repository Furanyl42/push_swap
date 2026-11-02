/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ops_push.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 12:00:00 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/02 12:00:00 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	pb(t_stack *a, t_stack *b)
{
	int	value;

	if (a->size == 0)
		return ;
	value = a->tab[a->head];
	a->head = (a->head + 1) % a->capacity;
	a->size--;
	b->head = (b->head - 1 + b->capacity) % b->capacity;
	b->tab[b->head] = value;
	b->size++;
	a->counts->pb++;
	if (!a->quiet)
		write(1, "pb\n", 3);
}

void	pa(t_stack *a, t_stack *b)
{
	int	value;

	if (b->size == 0)
		return ;
	value = b->tab[b->head];
	b->head = (b->head + 1) % b->capacity;
	b->size--;
	a->head = (a->head - 1 + a->capacity) % a->capacity;
	a->tab[a->head] = value;
	a->size++;
	a->counts->pa++;
	if (!a->quiet)
		write(1, "pa\n", 3);
}
