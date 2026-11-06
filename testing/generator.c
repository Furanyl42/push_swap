/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   generator.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesmet <adesmet@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 11:23:29 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/05 12:00:00 by adesmet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"
#include <stdlib.h>
#include <time.h>

static int	*shuffled_values(int min, int max)
{
	int	range;
	int	*all;
	int	i;
	int	j;
	int	tmp;

	range = max - min + 1;
	all = malloc(sizeof(int) * range);
	if (!all)
		return (NULL);
	i = -1;
	while (++i < range)
		all[i] = min + i;
	srand(time(NULL));
	i = range;
	while (--i > 0)
	{
		j = rand() % (i + 1);
		tmp = all[i];
		all[i] = all[j];
		all[j] = tmp;
	}
	return (all);
}

t_stack	*generate_random_stack(int n, int min, int max)
{
	t_stack	*stack;
	int		*all;
	int		i;

	if (n <= 0 || max - min + 1 < n)
		return (NULL);
	all = shuffled_values(min, max);
	if (!all)
		return (NULL);
	stack = init_stack(n);
	if (!stack)
	{
		free(all);
		return (NULL);
	}
	i = -1;
	while (++i < n)
		stack->tab[i] = all[i];
	stack->size = n;
	free(all);
	return (stack);
}
