/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   array_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 12:00:00 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/02 12:00:00 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static void	insertion_sort(int *array, int size)
{
	int	i;
	int	j;
	int	tmp;

	i = 1;
	while (i < size)
	{
		tmp = array[i];
		j = i - 1;
		while (j >= 0 && array[j] > tmp)
		{
			array[j + 1] = array[j];
			j--;
		}
		array[j + 1] = tmp;
		i++;
	}
}

int	*build_sorted_copy(t_stack *a)
{
	int	*copy;
	int	i;

	copy = malloc(sizeof(int) * a->size);
	if (!copy)
		return (NULL);
	i = 0;
	while (i < a->size)
	{
		copy[i] = get_stack_element(a, i);
		i++;
	}
	insertion_sort(copy, a->size);
	return (copy);
}
