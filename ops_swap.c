/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ops_swap.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 12:00:00 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/02 12:00:00 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	sa(t_stack *a)
{
	int	tmp;

	if (a->size < 2)
		return ;
	tmp = a->tab[a->head];
	a->tab[a->head] = a->tab[(a->head + 1) % a->capacity];
	a->tab[(a->head + 1) % a->capacity] = tmp;
	a->counts->sa++;
	if (!a->quiet)
		write(1, "sa\n", 3);
}

void	sb(t_stack *b)
{
	int	tmp;

	if (b->size < 2)
		return ;
	tmp = b->tab[b->head];
	b->tab[b->head] = b->tab[(b->head + 1) % b->capacity];
	b->tab[(b->head + 1) % b->capacity] = tmp;
	b->counts->sb++;
	if (!b->quiet)
		write(1, "sb\n", 3);
}

void	ss(t_stack *a, t_stack *b)
{
	int	tmp;

	if (a->size < 2 && b->size < 2)
		return ;
	if (a->size >= 2)
	{
		tmp = a->tab[a->head];
		a->tab[a->head] = a->tab[(a->head + 1) % a->capacity];
		a->tab[(a->head + 1) % a->capacity] = tmp;
	}
	if (b->size >= 2)
	{
		tmp = b->tab[b->head];
		b->tab[b->head] = b->tab[(b->head + 1) % b->capacity];
		b->tab[(b->head + 1) % b->capacity] = tmp;
	}
	a->counts->ss++;
	if (!a->quiet)
		write(1, "ss\n", 3);
}
