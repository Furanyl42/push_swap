/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 12:00:00 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/02 12:00:00 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	is_space(char c)
{
	return (c == ' ' || (c >= 9 && c <= 13));
}

int	token_count(char *s)
{
	int	count;
	int	i;

	count = 0;
	i = 0;
	while (s[i])
	{
		while (s[i] && is_space(s[i]))
			i++;
		if (s[i])
			count++;
		while (s[i] && !is_space(s[i]))
			i++;
	}
	return (count);
}

static int	read_digits(char *s, int *pos, long *res)
{
	*res = 0;
	while (s[*pos] >= '0' && s[*pos] <= '9')
	{
		if (*res > 2147483648L)
			*res = 2147483649L;
		else
			*res = *res * 10 + (s[*pos] - '0');
		(*pos)++;
	}
	return (1);
}

int	parse_one(char *s, int *pos, long *value)
{
	int		sign;

	while (s[*pos] && is_space(s[*pos]))
		(*pos)++;
	if (!s[*pos])
		return (0);
	sign = 1;
	if (s[*pos] == '+' || s[*pos] == '-')
	{
		if (s[*pos] == '-')
			sign = -1;
		(*pos)++;
	}
	if (s[*pos] < '0' || s[*pos] > '9')
		return (-1);
	read_digits(s, pos, value);
	if (s[*pos] && !is_space(s[*pos]))
		return (-1);
	*value = *value * sign;
	if (*value < -2147483648L || *value > 2147483647L)
		return (-1);
	return (1);
}
