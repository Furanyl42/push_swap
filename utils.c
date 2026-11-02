/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 12:00:00 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/02 12:00:00 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	get_cost(int index, int size)
{
	if (size <= 1)
		return (0);
	if (index <= size / 2)
		return (index);
	return (index - size);
}

void	init_counts(t_counts *counts)
{
	counts->sa = 0;
	counts->sb = 0;
	counts->ss = 0;
	counts->pa = 0;
	counts->pb = 0;
	counts->ra = 0;
	counts->rb = 0;
	counts->rr = 0;
	counts->rra = 0;
	counts->rrb = 0;
	counts->rrr = 0;
}

int	total_ops(t_counts *counts)
{
	return (counts->sa + counts->sb + counts->ss + counts->pa + counts->pb
		+ counts->ra + counts->rb + counts->rr + counts->rra + counts->rrb
		+ counts->rrr);
}
