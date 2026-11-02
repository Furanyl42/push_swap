/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stack.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 12:00:00 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/02 12:00:00 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	get_stack_element(t_stack *s, int index)
{
	if (index < 0 || index >= s->size)
		return (0);
	return (s->tab[(s->head + index) % s->capacity]);
}

int	has_duplicate(int *tab, int size, int value)
{
	int	i;

	i = 0;
	while (i < size)
	{
		if (tab[i] == value)
			return (1);
		i++;
	}
	return (0);
}

t_stack	*init_stack(int capacity, t_counts *counts)
{
	t_stack	*s;

	s = malloc(sizeof(t_stack));
	if (!s)
		return (NULL);
	s->tab = malloc(sizeof(int) * capacity);
	if (!s->tab)
	{
		free(s);
		return (NULL);
	}
	s->capacity = capacity;
	s->size = 0;
	s->head = 0;
	s->quiet = 0;
	s->counts = counts;
	return (s);
}

void	free_stack(t_stack *s)
{
	if (!s)
		return ;
	free(s->tab);
	free(s);
}
