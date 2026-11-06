/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_unsigned_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 17:00:46 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/20 17:00:51 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	unsigned_len(unsigned int n)
{
	int	len;

	len = 0;
	if (n == 0)
		return (1);
	while (n > 0)
	{
		n /= 10;
		len++;
	}
	return (len);
}

int	putnbr_unsigned(unsigned int n)
{
	int	count;

	count = 0;
	if (n > 9)
		count += putnbr_unsigned(n / 10);
	count += write(1, &"0123456789"[n % 10], 1);
	return (count);
}
