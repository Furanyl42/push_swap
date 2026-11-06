/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_print_ptr.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 16:26:04 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/26 12:20:21 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ptr_len(unsigned long n);
int	put_ptr_hex(unsigned long n);

static int	print_nil(t_flags *fmt)
{
	int	width;
	int	padding;
	int	count;

	width = fmt->width;
	if (width < 5)
		width = 5;
	padding = width - 5;
	count = 0;
	if (!fmt->minus)
	{
		while (padding-- > 0)
			count += write(1, " ", 1);
	}
	count += write(1, "(nil)", 5);
	if (fmt->minus)
	{
		while (padding-- > 0)
			count += write(1, " ", 1);
	}
	return (count);
}

static int	print_ptr_content(unsigned long addr)
{
	int	count;

	count = 0;
	count += write(1, "0x", 2);
	count += put_ptr_hex(addr);
	return (count);
}

static int	print_valid_ptr(unsigned long addr, t_flags *fmt)
{
	int		width;
	int		count;
	int		len;
	char	pad;

	len = ptr_len(addr) + 2;
	width = fmt->width;
	if (width < len)
		width = len;
	pad = ' ';
	if (fmt->zero && !fmt->minus)
		pad = '0';
	count = 0;
	if (!fmt->minus)
	{
		while (count < width - len)
			count += write(1, &pad, 1);
	}
	count += print_ptr_content(addr);
	if (fmt->minus)
	{
		while (count < width)
			count += write(1, " ", 1);
	}
	return (count);
}

int	print_ptr(va_list args, t_flags *fmt)
{
	void			*ptr;
	unsigned long	addr;

	ptr = va_arg(args, void *);
	if (!ptr)
		return (print_nil(fmt));
	addr = (unsigned long)ptr;
	return (print_valid_ptr(addr, fmt));
}
