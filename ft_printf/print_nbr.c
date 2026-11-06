/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_print_nbr.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 14:53:42 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/26 12:31:08 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	putnbr_len(long n);
int	digitlen(long n);
int	print_tnum_left(t_number nbr, int prec_zeros, int is_empty_zero);
int	print_tnum_zero(t_number nbr, int prec_zeros, int is_empty_zero);
int	print_tnum_right(t_number nbr, int prec_zeros, int is_empty_zero);

static void	init_nbr_struct(va_list args, t_flags *fmt, t_number *nbr)
{
	nbr->value = va_arg(args, int);
	if (nbr->value < 0)
	{
		nbr->sign = '-';
		nbr->isneg = 1;
	}
	else
	{
		nbr->sign = '+';
		nbr->isneg = 0;
	}
	if (fmt->plus)
		nbr->printsign = 1;
	else
		nbr->printsign = 0;
	nbr->digit_len = digitlen(nbr->value);
	nbr->width = fmt->width;
	if ((nbr->printsign || nbr->sign == '-') && nbr->width < nbr->digit_len + 1)
		nbr->width = nbr->digit_len;
	if (!nbr->printsign && nbr->sign == '+' && nbr->width < nbr->digit_len)
		nbr->width = nbr->digit_len;
}

static int	print_tnum(t_number nbr, t_flags *fmt)
{
	int	prec_zeros;
	int	num_len;
	int	is_empty_zero;
	int	zero_pad;

	num_len = nbr.digit_len;
	is_empty_zero = 0;
	if (fmt->dot && fmt->precision == 0 && nbr.value == 0)
		is_empty_zero = 1;
	if (is_empty_zero)
		num_len = 0;
	prec_zeros = 0;
	if (fmt->dot && fmt->precision > num_len)
		prec_zeros = fmt->precision - num_len;
	zero_pad = 0;
	if (fmt->zero && !fmt->minus && !fmt->dot)
		zero_pad = 1;
	if (fmt->minus)
		return (print_tnum_left(nbr, prec_zeros, is_empty_zero));
	if (zero_pad)
		return (print_tnum_zero(nbr, prec_zeros, is_empty_zero));
	return (print_tnum_right(nbr, prec_zeros, is_empty_zero));
}

int	print_nbr(va_list args, t_flags *fmt)
{
	t_number	nbr;

	init_nbr_struct(args, fmt, &nbr);
	return (print_tnum(nbr, fmt));
}
