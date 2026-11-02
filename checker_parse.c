/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checker_parse.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 12:00:00 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/02 12:00:00 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

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

static int	checker_count(int argc, char **argv)
{
	int	count;
	int	i;

	count = 0;
	i = 1;
	while (i < argc)
		count += token_count(argv[i++]);
	if (count < 1)
		count = 1;
	return (count);
}

t_stack	*parse_checker(int argc, char **argv, t_counts *counts)
{
	t_stack	*a;
	int		index;
	int		i;

	a = init_stack(checker_count(argc, argv), counts);
	if (!a)
		return (NULL);
	index = 0;
	i = 0;
	while (++i < argc)
	{
		if (!fill_arg(a, argv[i], &index))
		{
			free_stack(a);
			return (NULL);
		}
	}
	a->size = index;
	return (a);
}
