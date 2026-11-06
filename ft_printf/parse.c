/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_parse.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 17:14:20 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/26 12:17:57 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static void	parse_flags(const char **format, t_flags *fmt)
{
	while (**format && (**format == '-' || **format == '0' || **format == '#'
			|| **format == ' ' || **format == '+'))
	{
		if (**format == '-')
			fmt->minus = 1;
		else if (**format == '0')
			fmt->zero = 1;
		else if (**format == '#')
			fmt->hash = 1;
		else if (**format == ' ')
			fmt->space = 1;
		else if (**format == '+')
			fmt->plus = 1;
		(*format)++;
	}
}

static void	parse_width_and_prec(const char **format, t_flags *fmt)
{
	while (**format >= '0' && **format <= '9')
	{
		fmt->width = (fmt->width * 10) + (**format - '0');
		(*format)++;
	}
	if (**format == '.')
	{
		fmt->dot = 1;
		(*format)++;
		while (**format >= '0' && **format <= '9')
		{
			fmt->precision = (fmt->precision * 10) + (**format - '0');
			(*format)++;
		}
	}
}

void	parse_format(const char **format, t_flags *fmt)
{
	parse_flags(format, fmt);
	parse_width_and_prec(format, fmt);
	if (**format)
		fmt->specifier = **format;
}
