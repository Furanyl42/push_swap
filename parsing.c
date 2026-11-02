/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 12:00:00 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/02 12:00:00 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	is_flag_token(char *arg)
{
	if (ft_strncmp(arg, "--simple", 9) == 0)
		return (1);
	if (ft_strncmp(arg, "--medium", 9) == 0)
		return (1);
	if (ft_strncmp(arg, "--complex", 10) == 0)
		return (1);
	if (ft_strncmp(arg, "--adaptive", 11) == 0)
		return (1);
	if (ft_strncmp(arg, "--bench", 8) == 0)
		return (1);
	return (0);
}

int	parse_flags(int argc, char **argv, t_opts *opts)
{
	int	i;

	i = 1;
	while (i < argc)
	{
		if (ft_strncmp(argv[i], "--simple", 9) == 0)
			opts->strategy = STRAT_SIMPLE;
		else if (ft_strncmp(argv[i], "--medium", 9) == 0)
			opts->strategy = STRAT_MEDIUM;
		else if (ft_strncmp(argv[i], "--complex", 10) == 0)
			opts->strategy = STRAT_COMPLEX;
		else if (ft_strncmp(argv[i], "--adaptive", 11) == 0)
			opts->strategy = STRAT_ADAPTIVE;
		else if (ft_strncmp(argv[i], "--bench", 8) == 0)
			opts->bench = 1;
		else if (argv[i][0] == '-' && argv[i][1] == '-')
			return (0);
		i++;
	}
	return (1);
}

static t_stack	*init_parsed(int count)
{
	t_stack	*a;

	a = malloc(sizeof(t_stack));
	if (!a)
		return (NULL);
	a->tab = NULL;
	a->capacity = count;
	a->size = count;
	a->head = 0;
	a->quiet = 0;
	a->counts = NULL;
	if (count > 0)
	{
		a->tab = malloc(sizeof(int) * count);
		if (!a->tab)
		{
			free(a);
			return (NULL);
		}
	}
	return (a);
}

t_stack	*parse_input(int argc, char **argv, t_opts *opts)
{
	t_stack	*a;
	int		count;

	opts->strategy = STRAT_ADAPTIVE;
	opts->bench = 0;
	if (!parse_flags(argc, argv, opts))
		return (NULL);
	count = count_numbers(argc, argv);
	a = init_parsed(count);
	if (!a)
		return (NULL);
	if (!fill_stack(a, argc, argv))
	{
		free_stack(a);
		return (NULL);
	}
	return (a);
}
