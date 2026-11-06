/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_char.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 12:54:43 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/20 16:18:22 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	print_char(va_list args, t_flags *fmt)
{
	unsigned char	c;
	int				count;
	int				width;

	c = (char)va_arg(args, int);
	count = 0;
	width = fmt->width;
	if (width <= 0)
		width = 1;
	if (!fmt->minus)
	{
		while (count < width - 1)
			count += write(1, " ", 1);
	}
	count += write(1, &c, 1);
	if (fmt->minus)
	{
		while (count < width)
			count += write(1, " ", 1);
	}
	return (count);
}
