/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bench.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 12:00:00 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/02 12:00:00 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static void	put_strategy(t_strategy strat)
{
	if (strat == STRAT_SIMPLE)
		ft_putstr_fd("Simple (O(n^2))", 2);
	else if (strat == STRAT_MEDIUM)
		ft_putstr_fd("Medium (O(n*sqrt(n)))", 2);
	else if (strat == STRAT_COMPLEX)
		ft_putstr_fd("Complex (O(n log n))", 2);
	else
		ft_putstr_fd("Adaptive", 2);
}

static void	put_op_count(char *name, int value)
{
	ft_putstr_fd(name, 2);
	ft_putstr_fd(": ", 2);
	ft_putnbr_fd(value, 2);
	ft_putstr_fd("\n", 2);
}

void	print_bench(t_counts *counts, double disorder,
		t_strategy requested, t_strategy resolved)
{
	ft_putstr_fd("disorder: ", 2);
	put_pct(disorder);
	ft_putstr_fd("\nstrategy: ", 2);
	if (requested == STRAT_ADAPTIVE)
		ft_putstr_fd("Adaptive -> ", 2);
	put_strategy(resolved);
	ft_putstr_fd("\ntotal operations: ", 2);
	ft_putnbr_fd(total_ops(counts), 2);
	ft_putstr_fd("\n", 2);
	put_op_count("sa", counts->sa);
	put_op_count("sb", counts->sb);
	put_op_count("ss", counts->ss);
	put_op_count("pa", counts->pa);
	put_op_count("pb", counts->pb);
	put_op_count("ra", counts->ra);
	put_op_count("rb", counts->rb);
	put_op_count("rr", counts->rr);
	put_op_count("rra", counts->rra);
	put_op_count("rrb", counts->rrb);
	put_op_count("rrr", counts->rrr);
}
