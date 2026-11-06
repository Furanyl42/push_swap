/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_print_str.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:17:03 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/26 12:32:57 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static int	ft_strnlen(char *s, int max)
{
	int	i;

	i = 0;
	while (s[i] && i < max)
		i++;
	return (i);
}

static int	write_padded_str(char *s, int len, t_flags *fmt)
{
	int	padding;
	int	count;

	padding = 0;
	if (fmt->width > len)
		padding = fmt->width - len;
	count = 0;
	if (!fmt->minus)
	{
		while (padding-- > 0)
			count += write(1, " ", 1);
	}
	count += write(1, s, len);
	if (fmt->minus)
	{
		while (padding-- > 0)
			count += write(1, " ", 1);
	}
	return (count);
}

int	print_str(va_list args, t_flags *fmt)
{
	char	*s;
	int		is_null;
	int		len;

	s = va_arg(args, char *);
	is_null = 0;
	if (!s)
	{
		s = "(null)";
		is_null = 1;
	}
	if (fmt->dot)
	{
		if (is_null && fmt->precision < 6)
			len = 0;
		else
			len = ft_strnlen(s, fmt->precision);
	}
	else
	{
		len = 0;
		while (s[len])
			len++;
	}
	return (write_padded_str(s, len, fmt));
}
