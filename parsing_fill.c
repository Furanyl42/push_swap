/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_fill.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 12:00:00 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/02 12:00:00 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	count_numbers(int argc, char **argv)
{
	int	count;
	int	i;

	count = 0;
	i = 1;
	while (i < argc)
	{
		if (!is_flag_token(argv[i]))
			count += token_count(argv[i]);
		i++;
	}
	return (count);
}

static int	fill_arg(t_stack *a, char *arg, int *index)
{
	int		pos;
	int		err;
	long	value;

	pos = 0;
	err = parse_one(arg, &pos, &value);
	while (err == 1)
	{
		if (has_duplicate(a->tab, *index, (int)value))
			return (0);
		a->tab[*index] = (int)value;
		(*index)++;
		err = parse_one(arg, &pos, &value);
	}
	return (err == 0);
}

int	fill_stack(t_stack *a, int argc, char **argv)
{
	int	i;
	int	index;

	index = 0;
	i = 1;
	while (i < argc)
	{
		if (!is_flag_token(argv[i]) && !fill_arg(a, argv[i], &index))
			return (0);
		i++;
	}
	return (index == a->size);
}
