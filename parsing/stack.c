/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stack.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesmet <adesmet@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 15:34:48 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/05 12:00:00 by adesmet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	get_stack_element(t_stack *s, int index)
{
	if (index < 0 || index >= s->size)
		return (0);
	return (s->tab[(s->head + index) % s->capacity]);
}

t_count	*init_count(void)
{
	t_count	*count;

	count = malloc(sizeof(t_count));
	if (!count)
		return (NULL);
	count->sa = 0;
	count->sb = 0;
	count->ss = 0;
	count->pa = 0;
	count->pb = 0;
	count->ra = 0;
	count->rb = 0;
	count->rr = 0;
	count->rra = 0;
	count->rrb = 0;
	count->rrr = 0;
	return (count);
}

static int	alloc_stack(t_stack *s, int capacity)
{
	s->tab = malloc(sizeof(int) * capacity);
	s->idx = malloc(sizeof(int) * capacity);
	s->count = init_count();
	if (!s->tab || !s->idx || !s->count)
		return (0);
	return (1);
}

t_stack	*init_stack(int capacity)
{
	t_stack	*s;

	if (capacity < 1)
		capacity = 1;
	s = malloc(sizeof(t_stack));
	if (!s)
		return (NULL);
	s->tab = NULL;
	s->idx = NULL;
	s->count = NULL;
	s->capacity = capacity;
	s->size = 0;
	s->head = 0;
	if (!alloc_stack(s, capacity))
	{
		free_stack(s);
		return (NULL);
	}
	return (s);
}

void	free_stack(t_stack *s)
{
	if (!s)
		return ;
	free(s->tab);
	free(s->idx);
	free(s->count);
	free(s);
}
