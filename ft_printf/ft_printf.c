/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 10:58:22 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/27 14:10:09 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"
#include "libft.h"
#include <stddef.h>

int	dispatch_formatter(va_list args, t_flags *fmt)
{
	t_func			funcs[128];
	unsigned char	spec;

	if (!fmt)
		return (0);
	ft_bzero(funcs, sizeof(funcs));
	spec = (unsigned char)fmt->specifier;
	funcs['c'] = print_char;
	funcs['s'] = print_str;
	funcs['p'] = print_ptr;
	funcs['d'] = print_nbr;
	funcs['i'] = print_nbr;
	funcs['u'] = print_unsigned;
	funcs['x'] = print_hex_lower;
	funcs['X'] = print_hex_upper;
	funcs['%'] = print_percent;
	if (spec < 128 && funcs[spec])
		return (funcs[spec](args, fmt));
	return (0);
}

static int	parse_and_dispatch(const char **format, va_list args)
{
	t_flags	fmt;

	ft_bzero(&fmt, sizeof(t_flags));
	parse_format(format, &fmt);
	return (dispatch_formatter(args, &fmt));
}

static int	handle_directive(const char **format, va_list args, int *count)
{
	int	r;

	(*format)++;
	if (!**format)
		return (0);
	r = parse_and_dispatch(format, args);
	if (r == -1)
		return (-1);
	*count += r;
	return (1);
}

int	ft_printf(const char *format, ...)
{
	va_list	args;
	int		count;

	if (!format)
		return (-1);
	va_start(args, format);
	count = 0;
	while (*format)
	{
		if (*format == '%')
		{
			if (handle_directive(&format, args, &count) == -1)
			{
				va_end(args);
				return (-1);
			}
		}
		else
			count += write(1, format, 1);
		if (!*format)
			break ;
		format++;
	}
	va_end(args);
	return (count);
}
