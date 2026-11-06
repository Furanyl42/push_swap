/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checker_parse_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesmet <adesmet@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/05 13:00:00 by adesmet           #+#    #+#             */
/*   Updated: 2026/11/05 13:00:00 by adesmet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "checker_bonus.h"

static int	chk_token_count(const char *s)
{
	int	count;

	count = 0;
	while (*s)
	{
		while (is_space(*s))
			s++;
		if (*s)
			count++;
		while (*s && !is_space(*s))
			s++;
	}
	return (count);
}

static int	chk_count(int ac, char **av)
{
	int	total;
	int	i;

	total = 0;
	i = 1;
	while (i < ac)
	{
		total += chk_token_count(av[i]);
		i++;
	}
	return (total);
}

static int	chk_fill_arg(t_stack *a, const char *s, int *idx)
{
	int	val;

	while (*s)
	{
		while (is_space(*s))
			s++;
		if (!*s)
			break ;
		if (!parse_int(&s, &val) || has_duplicate(a->tab, *idx, val))
			return (0);
		a->idx[*idx] = *idx;
		a->tab[(*idx)++] = val;
	}
	return (1);
}

t_stack	*chk_parse(int ac, char **av)
{
	t_stack	*a;
	int		idx;
	int		i;

	a = init_stack(chk_count(ac, av));
	if (!a)
		return (NULL);
	idx = 0;
	i = 1;
	while (i < ac)
	{
		if (chk_token_count(av[i]) == 0 || !chk_fill_arg(a, av[i], &idx))
		{
			free_stack(a);
			return (NULL);
		}
		i++;
	}
	a->size = idx;
	return (a);
}
