/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   count.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesmet <adesmet@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/04 17:37:36 by adesmet           #+#    #+#             */
/*   Updated: 2026/11/05 12:00:00 by adesmet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	token_count(const char *s)
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
