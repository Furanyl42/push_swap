/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   disorder.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 12:00:00 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/02 12:00:00 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static void	compare_pairs(t_stack *a, int i, long *mistakes, long *pairs)
{
	int	j;

	j = i + 1;
	while (j < a->size)
	{
		(*pairs)++;
		if (get_stack_element(a, i) > get_stack_element(a, j))
			(*mistakes)++;
		j++;
	}
}

double	compute_disorder(t_stack *a)
{
	long	mistakes;
	long	pairs;
	int		i;

	if (a->size < 2)
		return (0.0);
	mistakes = 0;
	pairs = 0;
	i = 0;
	while (i < a->size)
	{
		compare_pairs(a, i, &mistakes, &pairs);
		i++;
	}
	return ((double)mistakes / (double)pairs);
}

int	is_sorted(t_stack *a)
{
	int	i;

	i = 0;
	while (i + 1 < a->size)
	{
		if (get_stack_element(a, i) > get_stack_element(a, i + 1))
			return (0);
		i++;
	}
	return (1);
}
