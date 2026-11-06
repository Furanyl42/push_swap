/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/13 17:52:05 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/26 12:34:30 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include <stdarg.h>
# include <stdlib.h>
# include <unistd.h>

typedef struct s_flags
{
	int		minus;
	int		zero;
	int		dot;
	int		width;
	int		precision;
	int		hash;
	int		space;
	int		plus;
	char	specifier;
}			t_flags;

typedef struct s_number
{
	char	sign;
	int		isneg;
	int		printsign;
	long	value;
	int		digit_len;
	int		width;
	char	*base;
	int		base_len;
}			t_number;

typedef int	(*t_func)(va_list, t_flags *);

int			print_percent(va_list args, t_flags *fmt);
int			print_char(va_list args, t_flags *fmt);
int			print_str(va_list args, t_flags *fmt);
int			print_nbr(va_list args, t_flags *fmt);
int			print_unsigned(va_list args, t_flags *fmt);
int			print_hex_lower(va_list args, t_flags *fmt);
int			print_hex_upper(va_list args, t_flags *fmt);
int			print_ptr(va_list args, t_flags *fmt);
void		parse_format(const char **format, t_flags *fmt);
int			dispatch_formatter(va_list args, t_flags *fmt);

int			ft_printf(const char *format, ...);

#endif
