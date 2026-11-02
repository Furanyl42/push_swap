/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_dispatcher.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 12:00:00 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/02 12:00:00 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

t_strategy	resolve_strategy(t_strategy requested, double disorder)
{
	if (requested != STRAT_ADAPTIVE)
		return (requested);
	if (disorder < 0.2)
		return (STRAT_SIMPLE);
	if (disorder < 0.5)
		return (STRAT_MEDIUM);
	return (STRAT_COMPLEX);
}

void	sort_stacks(t_stack *a, t_stack *b, t_strategy strat)
{
	if (a->size < 2)
		return ;
	if (strat == STRAT_SIMPLE)
		sort_simple(a, b);
	else if (strat == STRAT_MEDIUM)
		sort_medium(a, b);
	else if (strat == STRAT_COMPLEX)
		sort_complex(a, b);
}
