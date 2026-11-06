/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checker_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesmet <adesmet@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/05 13:00:00 by adesmet           #+#    #+#             */
/*   Updated: 2026/11/05 13:00:00 by adesmet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "checker_bonus.h"

static int	chk_error(void)
{
	ft_putstr_fd("Error\n", 2);
	return (1);
}

int	chk_sorted(t_stack *a)
{
	int	i;

	i = 1;
	while (i < a->size)
	{
		if (get_stack_element(a, i - 1) > get_stack_element(a, i))
			return (0);
		i++;
	}
	return (1);
}

static void	chk_finish(t_stack *a, t_stack *b)
{
	if (chk_sorted(a) && b->size == 0)
		ft_putstr_fd("OK\n", 1);
	else
		ft_putstr_fd("KO\n", 1);
	free_stack(a);
	free_stack(b);
}

int	main(int ac, char **av)
{
	t_stack	*a;
	t_stack	*b;

	if (ac < 2)
		return (0);
	a = chk_parse(ac, av);
	if (!a)
		return (chk_error());
	b = init_stack(a->capacity);
	if (!b)
	{
		free_stack(a);
		return (chk_error());
	}
	if (!chk_execute(a, b))
	{
		free_stack(a);
		free_stack(b);
		return (chk_error());
	}
	chk_finish(a, b);
	return (0);
}
