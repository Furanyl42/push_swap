/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesmet <adesmet@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 14:24:02 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/05 12:00:00 by adesmet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	fill_arg(t_stack *a, const char *s, int *idx)
{
	int	val;
	int	found;

	found = 0;
	while (*s)
	{
		while (is_space(*s))
			s++;
		if (!*s)
			break ;
		if (!parse_int(&s, &val) || has_duplicate(a->tab, *idx, val))
			return (0);
		a->tab[(*idx)++] = val;
		found = 1;
	}
	return (found);
}

int	fill_stack(t_stack *a, int argc, char **argv)
{
	int	i;
	int	idx;

	i = 1;
	idx = 0;
	while (i < argc)
	{
		if (!is_flag_token(argv[i]) && !fill_arg(a, argv[i], &idx))
			return (0);
		i++;
	}
	a->size = idx;
	return (1);
}

t_stack	*parse_input(int argc, char **argv, t_opts *opts)
{
	t_stack	*a;

	opts->strategy = STRAT_ADAPTIVE;
	opts->bench = 0;
	if (!parse_flags(argc, argv, opts))
		return (NULL);
	a = init_stack(count_numbers(argc, argv));
	if (!a)
		return (NULL);
	if (!fill_stack(a, argc, argv))
	{
		free_stack(a);
		return (NULL);
	}
	return (a);
}
