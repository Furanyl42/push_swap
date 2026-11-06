/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   disorder.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 14:39:38 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/05 12:00:00 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

double	compute_disorder(t_stack a)
{
	int		i;
	int		j;
	long	mistakes;
	long	pairs;

	if (a.size <= 1)
		return (0.0);
	mistakes = 0;
	pairs = 0;
	i = 0;
	while (i < a.size)
	{
		j = i + 1;
		while (j < a.size)
		{
			pairs++;
			if (get_stack_element(&a, i) > get_stack_element(&a, j))
				mistakes++;
			j++;
		}
		i++;
	}
	if (pairs == 0)
		return (0.0);
	return ((double)mistakes / (double)pairs);
}

t_strategy	strategy_from_disorder(double disorder)
{
	if (disorder < 0.2)
		return (STRAT_SIMPLE);
	if (disorder < 0.5)
		return (STRAT_MEDIUM);
	return (STRAT_COMPLEX);
}
