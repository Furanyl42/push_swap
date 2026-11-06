/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesmet <adesmet@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/05 12:00:00 by adesmet           #+#    #+#             */
/*   Updated: 2026/11/05 12:00:00 by adesmet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf/ft_printf.h"
#include "test.h"

void	print_cell(t_stack *s, int i)
{
	if (i < s->size)
		ft_printf("%7d", get_stack_element(s, i));
	else
		ft_printf("       ");
}

void	print_row(t_stack *a, t_stack *b, int i)
{
	ft_printf("│ ");
	print_cell(a, i);
	ft_printf(" │ ");
	print_cell(b, i);
	ft_printf(" │\n");
}

void	print_op(const char *name, int value)
{
	ft_printf("│  %-3s : %-20d │\n", name, value);
}

void	print_first(t_count *c)
{
	print_op("sa", c->sa);
	print_op("sb", c->sb);
	print_op("ss", c->ss);
	ft_printf("├─────────────────────────────┤\n");
	print_op("pa", c->pa);
	print_op("pb", c->pb);
	ft_printf("├─────────────────────────────┤\n");
}

void	print_second(t_count *c)
{
	print_op("ra", c->ra);
	print_op("rb", c->rb);
	print_op("rr", c->rr);
	ft_printf("├─────────────────────────────┤\n");
	print_op("rra", c->rra);
	print_op("rrb", c->rrb);
	print_op("rrr", c->rrr);
	ft_printf("├─────────────────────────────┤\n");
}
