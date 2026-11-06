/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bench.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesmet <adesmet@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/05 12:00:00 by adesmet           #+#    #+#             */
/*   Updated: 2026/11/05 12:00:00 by adesmet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bench.h"

void	merge_counts(t_count *dst, t_count *src)
{
	if (!dst || !src || dst == src)
		return ;
	dst->sa += src->sa;
	dst->sb += src->sb;
	dst->ss += src->ss;
	dst->pa += src->pa;
	dst->pb += src->pb;
	dst->ra += src->ra;
	dst->rb += src->rb;
	dst->rr += src->rr;
	dst->rra += src->rra;
	dst->rrb += src->rrb;
	dst->rrr += src->rrr;
}

int	get_total_operations(t_count *count)
{
	if (!count)
		return (0);
	return (count->sa + count->sb + count->ss + count->pa + count->pb
		+ count->ra + count->rb + count->rr + count->rra + count->rrb
		+ count->rrr);
}

static void	print_line(const char *label, int value)
{
	put_str(label);
	put_nbr(value);
	put_str("\n");
}

static void	print_counts(t_count *count)
{
	print_line("sa: ", count->sa);
	print_line("sb: ", count->sb);
	print_line("ss: ", count->ss);
	print_line("pa: ", count->pa);
	print_line("pb: ", count->pb);
	print_line("ra: ", count->ra);
	print_line("rb: ", count->rb);
	print_line("rr: ", count->rr);
	print_line("rra: ", count->rra);
	print_line("rrb: ", count->rrb);
	print_line("rrr: ", count->rrr);
}

void	print_bench(t_strategy strategy, double disorder, t_count *count)
{
	put_str("Disorder: ");
	put_disorder(disorder);
	put_str("Strategy: ");
	put_str(strategy_name(strategy));
	put_str("\nComplexity: ");
	put_str(strategy_complexity(strategy));
	put_str("\nTotal operations: ");
	put_nbr(get_total_operations(count));
	put_str("\n");
	print_counts(count);
}
