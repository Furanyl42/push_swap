/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checker_main.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 12:00:00 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/02 12:00:00 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static int	finish(t_stack *a, t_stack *b)
{
	if (is_sorted(a) && b->size == 0)
		write(1, "OK\n", 3);
	else
		write(1, "KO\n", 3);
	free_stack(a);
	free_stack(b);
	return (0);
}

int	checker_error(t_stack *a, t_stack *b)
{
	free_stack(a);
	free_stack(b);
	write(2, "Error\n", 6);
	return (1);
}

int	main(int argc, char **argv)
{
	t_stack		*a;
	t_stack		*b;
	t_counts	counts;

	if (argc < 2)
		return (0);
	init_counts(&counts);
	a = parse_checker(argc, argv, &counts);
	if (!a)
		return (checker_error(NULL, NULL));
	b = init_stack(a->capacity, &counts);
	if (!b)
		return (checker_error(a, NULL));
	a->quiet = 1;
	b->quiet = 1;
	if (!run_instructions(a, b))
		return (checker_error(a, b));
	return (finish(a, b));
}
