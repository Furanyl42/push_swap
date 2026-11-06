/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   numbers.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesmet <adesmet@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/04 17:32:48 by adesmet           #+#    #+#             */
/*   Updated: 2026/11/05 12:00:00 by adesmet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	is_space(char c)
{
	return (c == ' ' || (c >= 9 && c <= 13));
}

int	read_sign(const char **s)
{
	int	sign;

	sign = 1;
	if (**s == '-')
		sign = -1;
	if (**s == '-' || **s == '+')
		(*s)++;
	return (sign);
}

int	parse_int(const char **s, int *out)
{
	long	n;
	int		sign;

	sign = read_sign(s);
	if (!ft_isdigit(**s))
		return (0);
	n = 0;
	while (ft_isdigit(**s))
	{
		n = n * 10 + (**s - '0');
		if (n > (long)INT_MAX + 1)
			return (0);
		(*s)++;
	}
	n *= sign;
	if (n > INT_MAX || n < INT_MIN || (**s && !is_space(**s)))
		return (0);
	*out = (int)n;
	return (1);
}

int	has_duplicate(int *tab, int size, int val)
{
	while (size-- > 0)
	{
		if (tab[size] == val)
			return (1);
	}
	return (0);
}
