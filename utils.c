/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 16:49:18 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/19 17:17:05 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"
#include "libft/libft.h"

void print_stacks(t_stack a, t_stack b)
{
	int i;

	if (a.capacity != b.capacity)
		return ;

	i = 0;
	while (i < a.capacity)
	{
		ft_putnbr_fd(a.tab[(a.head + i) % a.capacity], 1);
		ft_putstr_fd(" ", 1);
		ft_putnbr_fd(b.tab[(b.head + i) % b.capacity], 1);
		ft_putstr_fd("\n", 1);
		i++;
	}
}
