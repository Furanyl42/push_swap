/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 18:57:19 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/15 12:58:41 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_putnbr_fd(int n, int fd)
{
	long	v;

	v = (long)n;
	if (v < 0)
	{
		ft_putchar_fd('-', fd);
		v = -v;
	}
	if (v > 9)
		ft_putnbr_fd(v / 10, fd);
	ft_putchar_fd((v % 10) + '0', fd);
}
