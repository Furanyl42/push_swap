/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   medium.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesmet <adesmet@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 15:56:25 by adesmet           #+#    #+#             */
/*   Updated: 2026/11/05 12:00:00 by adesmet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	index_stack(t_stack *a)
{
	int	*copy;
	int	i;
	int	j;

	copy = malloc(sizeof(int) * a->size);
	if (!copy)
		return ;
	i = -1;
	while (++i < a->size)
		copy[i] = get_stack_element(a, i);
	insertion_sort(copy, a->size);
	i = -1;
	while (++i < a->size)
	{
		j = -1;
		while (++j < a->size)
		{
			if (get_stack_element(a, i) == copy[j])
			{
				a->idx[(i + a->head) % a->capacity] = j;
				break ;
			}
		}
	}
	free(copy);
}

void	push_chunks(t_stack *a, t_stack *b, int size)
{
	int	i;
	int	chunk_size;

	i = 0;
	chunk_size = get_chunk_size(size, 150);
	while (a->size > 0)
	{
		if (a->idx[a->head] <= i)
		{
			pb(a, b);
			rb(b);
			i++;
		}
		else if (a->idx[a->head] <= i + chunk_size)
		{
			pb(a, b);
			i++;
		}
		else
			ra(a);
	}
}

int	get_max_pos(t_stack *b)
{
	int	i;
	int	max_pos;
	int	max_val;
	int	cur_val;

	i = 0;
	max_pos = 0;
	max_val = get_stack_element(b, 0);
	while (i < b->size)
	{
		cur_val = get_stack_element(b, i);
		if (cur_val > max_val)
		{
			max_val = cur_val;
			max_pos = i;
		}
		i++;
	}
	return (max_pos);
}

void	push_to_a(t_stack *a, t_stack *b)
{
	int	max_pos;

	while (b->size > 0)
	{
		max_pos = get_max_pos(b);
		if (max_pos <= b->size / 2)
		{
			while (max_pos-- > 0)
				rb(b);
		}
		else
		{
			while (max_pos++ < b->size)
				rrb(b);
		}
		pa(a, b);
	}
}

void	medium_sort(t_stack *a, t_stack *b)
{
	index_stack(a);
	push_chunks(a, b, a->size);
	push_to_a(a, b);
}
