/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_print_unsigned.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 15:32:33 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/26 12:34:10 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	unsigned_len(unsigned int n);
int	putnbr_unsigned(unsigned int n);

static int	print_unsigned_left(unsigned int nbr, t_flags *fmt,
								int prec_zeros, int is_empty_zero)
{
	int	count;

	count = 0;
	while (prec_zeros-- > 0)
		count += write(1, "0", 1);
	if (!is_empty_zero)
		count += putnbr_unsigned(nbr);
	while (count < fmt->width)
		count += write(1, " ", 1);
	return (count);
}

static int	print_unsigned_zero(unsigned int nbr, t_flags *fmt,
								int prec_zeros, int is_empty_zero)
{
	int	count;
	int	num_len;
	int	padding;

	count = 0;
	num_len = unsigned_len(nbr);
	if (is_empty_zero)
		num_len = 0;
	padding = 0;
	if (fmt->width > (prec_zeros + num_len))
		padding = fmt->width - (prec_zeros + num_len);
	while (padding-- > 0)
		count += write(1, "0", 1);
	while (prec_zeros-- > 0)
		count += write(1, "0", 1);
	if (!is_empty_zero)
		count += putnbr_unsigned(nbr);
	return (count);
}

static int	print_unsigned_right(unsigned int nbr, t_flags *fmt,
								int prec_zeros, int is_empty_zero)
{
	int	count;
	int	num_len;
	int	padding;

	count = 0;
	num_len = unsigned_len(nbr);
	if (is_empty_zero)
		num_len = 0;
	padding = 0;
	if (fmt->width > (prec_zeros + num_len))
		padding = fmt->width - (prec_zeros + num_len);
	while (padding-- > 0)
		count += write(1, " ", 1);
	while (prec_zeros-- > 0)
		count += write(1, "0", 1);
	if (!is_empty_zero)
		count += putnbr_unsigned(nbr);
	return (count);
}

int	print_unsigned(va_list args, t_flags *fmt)
{
	unsigned int	nbr;
	int				num_len;
	int				prec_zeros;
	int				is_empty_zero;

	nbr = va_arg(args, unsigned int);
	num_len = unsigned_len(nbr);
	is_empty_zero = 0;
	if (fmt->dot && fmt->precision == 0 && nbr == 0)
		is_empty_zero = 1;
	if (is_empty_zero)
		num_len = 0;
	prec_zeros = 0;
	if (fmt->dot && fmt->precision > num_len)
		prec_zeros = fmt->precision - num_len;
	if (fmt->minus)
		return (print_unsigned_left(nbr, fmt, prec_zeros, is_empty_zero));
	if (fmt->zero && !fmt->minus && !fmt->dot)
		return (print_unsigned_zero(nbr, fmt, prec_zeros, is_empty_zero));
	return (print_unsigned_right(nbr, fmt, prec_zeros, is_empty_zero));
}
