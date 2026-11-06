/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_hex_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 11:59:15 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/26 12:26:13 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

#include "ft_printf.h"

int	putnbr_hex(unsigned int n, int uppercase);

int	hex_len(unsigned int n)
{
	int	len;

	len = 0;
	if (n == 0)
		return (1);
	while (n > 0)
	{
		n /= 16;
		len++;
	}
	return (len);
}

void	get_hex_details(unsigned int nbr, t_flags *fmt,
							int *num_len, int *prec_zeros)
{
	*num_len = hex_len(nbr);
	if (fmt->dot && fmt->precision == 0 && nbr == 0)
		*num_len = 0;
	*prec_zeros = 0;
	if (fmt->dot && fmt->precision > *num_len)
		*prec_zeros = fmt->precision - *num_len;
}

int	print_hex_left(unsigned int nbr, t_flags *fmt,
						int uppercase, int has_prefix)
{
	int	count;
	int	prec_zeros;
	int	num_len;

	count = 0;
	get_hex_details(nbr, fmt, &num_len, &prec_zeros);
	if (has_prefix && uppercase)
		count += write(1, "0X", 2);
	else if (has_prefix)
		count += write(1, "0x", 2);
	while (prec_zeros-- > 0)
		count += write(1, "0", 1);
	if (!(fmt->dot && fmt->precision == 0 && nbr == 0))
		count += putnbr_hex(nbr, uppercase);
	while (count < fmt->width)
		count += write(1, " ", 1);
	return (count);
}

int	print_hex_zero(unsigned int nbr, t_flags *fmt,
						int uppercase, int has_prefix)
{
	int	count;
	int	prec_zeros;
	int	num_len;
	int	padding;

	count = 0;
	get_hex_details(nbr, fmt, &num_len, &prec_zeros);
	padding = fmt->width - (has_prefix + prec_zeros + num_len);
	if (has_prefix && uppercase)
		count += write(1, "0X", 2);
	else if (has_prefix)
		count += write(1, "0x", 2);
	while (padding-- > 0)
		count += write(1, "0", 1);
	while (prec_zeros-- > 0)
		count += write(1, "0", 1);
	if (!(fmt->dot && fmt->precision == 0 && nbr == 0))
		count += putnbr_hex(nbr, uppercase);
	return (count);
}

int	print_hex_right(unsigned int nbr, t_flags *fmt,
						int uppercase, int has_prefix)
{
	int	count;
	int	prec_zeros;
	int	num_len;
	int	padding;

	count = 0;
	get_hex_details(nbr, fmt, &num_len, &prec_zeros);
	padding = fmt->width - (has_prefix + prec_zeros + num_len);
	while (padding-- > 0)
		count += write(1, " ", 1);
	if (has_prefix && uppercase)
		count += write(1, "0X", 2);
	else if (has_prefix)
		count += write(1, "0x", 2);
	while (prec_zeros-- > 0)
		count += write(1, "0", 1);
	if (!(fmt->dot && fmt->precision == 0 && nbr == 0))
		count += putnbr_hex(nbr, uppercase);
	return (count);
}
