/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   flags.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesmet <adesmet@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/04 17:33:16 by adesmet           #+#    #+#             */
/*   Updated: 2026/11/05 12:00:00 by adesmet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	streq(const char *a, const char *b)
{
	return (ft_strncmp(a, b, ft_strlen(b) + 1) == 0);
}

int	apply_flag(const char *arg, t_opts *opts)
{
	if (streq(arg, "--simple"))
		opts->strategy = STRAT_SIMPLE;
	else if (streq(arg, "--medium"))
		opts->strategy = STRAT_MEDIUM;
	else if (streq(arg, "--complex"))
		opts->strategy = STRAT_COMPLEX;
	else if (streq(arg, "--adaptive"))
		opts->strategy = STRAT_ADAPTIVE;
	else if (streq(arg, "--bench"))
		opts->bench = 1;
	else
		return (0);
	return (1);
}

int	is_flag_token(const char *arg)
{
	return (arg[0] == '-' && arg[1] == '-');
}

int	parse_flags(int argc, char **argv, t_opts *opts)
{
	int	i;

	i = 1;
	while (i < argc)
	{
		if (is_flag_token(argv[i]) && !apply_flag(argv[i], opts))
			return (0);
		i++;
	}
	return (1);
}
