/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bench_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesmet <adesmet@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/05 12:00:00 by adesmet           #+#    #+#             */
/*   Updated: 2026/11/05 12:00:00 by adesmet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bench.h"

void	put_str(const char *s)
{
	while (*s)
		write(2, s++, 1);
}

void	put_nbr(long n)
{
	char	c;

	if (n < 0)
	{
		write(2, "-", 1);
		n = -n;
	}
	if (n >= 10)
		put_nbr(n / 10);
	c = '0' + (n % 10);
	write(2, &c, 1);
}

void	put_disorder(double disorder)
{
	long	hundredths;

	hundredths = (long)(disorder * 10000.0 + 0.5);
	put_nbr(hundredths / 100);
	put_str(".");
	if (hundredths % 100 < 10)
		put_str("0");
	put_nbr(hundredths % 100);
	put_str("%\n");
}

const char	*strategy_name(t_strategy strategy)
{
	if (strategy == STRAT_SIMPLE)
		return ("Simple");
	if (strategy == STRAT_MEDIUM)
		return ("Medium");
	if (strategy == STRAT_COMPLEX)
		return ("Complex");
	return ("Adaptive");
}

const char	*strategy_complexity(t_strategy strategy)
{
	if (strategy == STRAT_SIMPLE)
		return ("O(n^2)");
	if (strategy == STRAT_MEDIUM)
		return ("O(n sqrt n)");
	if (strategy == STRAT_COMPLEX)
		return ("O(n log n)");
	return ("O(n log n)");
}
