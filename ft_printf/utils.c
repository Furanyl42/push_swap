/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 16:39:33 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/20 16:45:48 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	putnbr_len(long n)
{
	int	count;

	count = 0;
	if (n < 0)
		n = -n;
	if (n > 9)
		count += putnbr_len(n / 10);
	count += write(1, &"0123456789"[n % 10], 1);
	return (count);
}

int	digitlen(long n)
{
	int	len;

	len = 0;
	while (n != 0)
	{
		n /= 10;
		len++;
	}
	return (len);
}

int	print_tnum_left(t_number nbr, int prec_zeros, int is_empty_zero)
{
	int	count;
	int	has_sign;

	count = 0;
	has_sign = (nbr.printsign || nbr.isneg);
	if (has_sign)
		count += write(1, &nbr.sign, 1);
	while (prec_zeros-- > 0)
		count += write(1, "0", 1);
	if (!is_empty_zero)
		count += putnbr_len(nbr.value);
	while (count < nbr.width)
		count += write(1, " ", 1);
	return (count);
}

int	print_tnum_zero(t_number nbr, int prec_zeros, int is_empty_zero)
{
	int	count;
	int	has_sign;
	int	content_len;
	int	padding;

	count = 0;
	has_sign = (nbr.printsign || nbr.isneg);
	content_len = has_sign + prec_zeros + nbr.digit_len;
	padding = 0;
	if (nbr.width > content_len)
		padding = nbr.width - content_len;
	if (has_sign)
		count += write(1, &nbr.sign, 1);
	while (padding-- > 0)
		count += write(1, "0", 1);
	while (prec_zeros-- > 0)
		count += write(1, "0", 1);
	if (!is_empty_zero)
		count += putnbr_len(nbr.value);
	return (count);
}

int	print_tnum_right(t_number nbr, int prec_zeros, int is_empty_zero)
{
	int	count;
	int	has_sign;
	int	content_len;
	int	padding;

	count = 0;
	has_sign = (nbr.printsign || nbr.isneg);
	content_len = has_sign + prec_zeros + nbr.digit_len;
	padding = 0;
	if (nbr.width > content_len)
		padding = nbr.width - content_len;
	while (padding-- > 0)
		count += write(1, " ", 1);
	if (has_sign)
		count += write(1, &nbr.sign, 1);
	while (prec_zeros-- > 0)
		count += write(1, "0", 1);
	if (!is_empty_zero)
		count += putnbr_len(nbr.value);
	return (count);
}
