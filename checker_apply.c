/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checker_apply.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 12:00:00 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/02 12:00:00 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static int	match(char *line, char *op)
{
	return (ft_strncmp(line, op, ft_strlen(op) + 1) == 0);
}

static int	apply_swap_push(char *line, t_stack *a, t_stack *b)
{
	if (match(line, "sa"))
		return (sa(a), 1);
	if (match(line, "sb"))
		return (sb(b), 1);
	if (match(line, "ss"))
		return (ss(a, b), 1);
	if (match(line, "pa"))
		return (pa(a, b), 1);
	if (match(line, "pb"))
		return (pb(a, b), 1);
	return (0);
}

static int	apply_rotate(char *line, t_stack *a, t_stack *b)
{
	if (match(line, "ra"))
		return (ra(a), 1);
	if (match(line, "rb"))
		return (rb(b), 1);
	if (match(line, "rr"))
		return (rr(a, b), 1);
	if (match(line, "rra"))
		return (rra(a), 1);
	if (match(line, "rrb"))
		return (rrb(b), 1);
	if (match(line, "rrr"))
		return (rrr(a, b), 1);
	return (0);
}

int	apply_op(char *line, t_stack *a, t_stack *b)
{
	if (apply_swap_push(line, a, b))
		return (1);
	return (apply_rotate(line, a, b));
}
