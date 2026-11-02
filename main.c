/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 12:00:00 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/02 12:00:00 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static int	error_exit(t_stack *a)
{
	free_stack(a);
	write(2, "Error\n", 6);
	return (1);
}

static int	run(t_stack *a, t_opts *opts)
{
	t_stack		*b;
	t_counts	counts;
	double		disorder;
	t_strategy	strat;

	init_counts(&counts);
	a->counts = &counts;
	b = init_stack(a->capacity, &counts);
	if (!b)
		return (0);
	disorder = compute_disorder(a);
	strat = resolve_strategy(opts->strategy, disorder);
	if (disorder > 0.0)
		sort_stacks(a, b, strat);
	if (opts->bench)
		print_bench(&counts, disorder, opts->strategy, strat);
	free_stack(b);
	return (1);
}

int	main(int argc, char **argv)
{
	t_stack	*a;
	t_opts	opts;

	if (argc < 2)
		return (0);
	a = parse_input(argc, argv, &opts);
	if (!a)
		return (error_exit(NULL));
	if (a->size == 0)
	{
		free_stack(a);
		return (0);
	}
	if (!run(a, &opts))
		return (error_exit(a));
	free_stack(a);
	return (0);
}
