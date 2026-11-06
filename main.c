/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesmet <adesmet@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 16:47:08 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/05 12:00:00 by adesmet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static int	error(void)
{
	ft_putstr_fd("Error\n", 2);
	return (1);
}

static int	is_sorted(t_stack *a)
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

static void	sort_with(t_stack *a, t_stack *b, t_strategy strategy)
{
	if (strategy == STRAT_SIMPLE)
		simple_sort(a, b);
	else if (strategy == STRAT_MEDIUM)
		medium_sort(a, b);
	else if (strategy == STRAT_COMPLEX)
		complex_sort(a, b);
}

static void	run(t_stack *a, t_stack *b, t_opts *opts)
{
	t_strategy	strategy;
	double		disorder;

	disorder = 0.0;
	if (opts->bench || opts->strategy == STRAT_ADAPTIVE)
		disorder = compute_disorder(*a);
	strategy = opts->strategy;
	if (strategy == STRAT_ADAPTIVE)
		strategy = strategy_from_disorder(disorder);
	if (a->size > 1 && !is_sorted(a))
		sort_with(a, b, strategy);
	if (opts->bench)
	{
		merge_counts(a->count, b->count);
		print_bench(strategy, disorder, a->count);
	}
}

int	main(int ac, char **av)
{
	t_stack	*a;
	t_stack	*b;
	t_opts	opts;

	if (ac < 2)
		return (0);
	a = parse_input(ac, av, &opts);
	if (!a)
		return (error());
	if (a->size == 0)
		return (free_stack(a), 0);
	b = init_stack(a->capacity);
	if (!b)
		return (free_stack(a), error());
	run(a, b, &opts);
	free_stack(a);
	free_stack(b);
	return (0);
}
