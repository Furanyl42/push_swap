/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bench_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 12:00:00 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/02 12:00:00 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	put_pct(double disorder)
{
	int	pct;

	pct = (int)(disorder * 10000.0 + 0.5);
	ft_putnbr_fd(pct / 100, 2);
	ft_putstr_fd(".", 2);
	if (pct % 100 < 10)
		ft_putstr_fd("0", 2);
	ft_putnbr_fd(pct % 100, 2);
	ft_putstr_fd("%", 2);
}
