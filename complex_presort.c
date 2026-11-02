/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   complex_presort.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 12:00:00 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/02 12:00:00 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	normalize_stack(t_stack *a)
{
	int	*sorted;
	int	index;
	int	i;

	sorted = build_sorted_copy(a);
	if (!sorted)
		return (0);
	i = 0;
	while (i < a->size)
	{
		index = (a->head + i) % a->capacity;
		a->tab[index] = find_rank(sorted, a->size, a->tab[index]);
		i++;
	}
	free(sorted);
	return (1);
}

static void	presort_one(t_stack *a, t_stack *b, int limit, int sink)
{
	if (get_stack_element(a, 0) < limit)
	{
		pb(a, b);
		if (get_stack_element(b, 0) < sink)
			rb(b);
	}
	else
		ra(a);
}

void	complex_presort(t_stack *a, t_stack *b, int div, int fr)
{
	int	chunk_size;
	int	chunk;
	int	count;
	int	i;

	chunk_size = a->size / div;
	if (chunk_size < 1)
		chunk_size = 1;
	chunk = 0;
	while (a->size > 0)
	{
		count = a->size;
		i = 0;
		while (i < count)
		{
			presort_one(a, b, (chunk + 1) * chunk_size,
				chunk * chunk_size + chunk_size / fr);
			i++;
		}
		chunk++;
	}
}
