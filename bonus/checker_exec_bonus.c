/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checker_exec_bonus.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesmet <adesmet@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/05 13:00:00 by adesmet           #+#    #+#             */
/*   Updated: 2026/11/05 13:00:00 by adesmet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "checker_bonus.h"

static int	chk_apply_swap(const char *line, t_stack *a, t_stack *b)
{
	if (chk_streq(line, "sa"))
		return (chk_swap(a), 1);
	if (chk_streq(line, "sb"))
		return (chk_swap(b), 1);
	if (chk_streq(line, "ss"))
		return (chk_swap(a), chk_swap(b), 1);
	if (chk_streq(line, "pa"))
		return (chk_push(a, b), 1);
	if (chk_streq(line, "pb"))
		return (chk_push(b, a), 1);
	return (0);
}

static int	chk_apply_rotate(const char *line, t_stack *a, t_stack *b)
{
	if (chk_streq(line, "ra"))
		return (chk_rotate(a), 1);
	if (chk_streq(line, "rb"))
		return (chk_rotate(b), 1);
	if (chk_streq(line, "rr"))
		return (chk_rotate(a), chk_rotate(b), 1);
	if (chk_streq(line, "rra"))
		return (chk_reverse(a), 1);
	if (chk_streq(line, "rrb"))
		return (chk_reverse(b), 1);
	if (chk_streq(line, "rrr"))
		return (chk_reverse(a), chk_reverse(b), 1);
	return (0);
}

int	chk_apply(const char *line, t_stack *a, t_stack *b)
{
	if (chk_apply_swap(line, a, b))
		return (1);
	return (chk_apply_rotate(line, a, b));
}

static int	chk_feed(t_reader *r, int n)
{
	int	i;

	i = 0;
	while (i < n)
	{
		if (r->buf[i] == '\n')
		{
			r->line[r->len] = '\0';
			if (r->len == 0 || !chk_apply(r->line, r->a, r->b))
				return (0);
			r->len = 0;
		}
		else
		{
			if (r->len >= 7)
				return (0);
			r->line[r->len++] = r->buf[i];
		}
		i++;
	}
	return (1);
}

int	chk_execute(t_stack *a, t_stack *b)
{
	t_reader	r;
	int			n;

	r.len = 0;
	r.a = a;
	r.b = b;
	n = read(0, r.buf, 4096);
	while (n > 0)
	{
		if (!chk_feed(&r, n))
			return (0);
		n = read(0, r.buf, 4096);
	}
	if (n < 0)
		return (0);
	if (r.len == 0)
		return (1);
	r.line[r.len] = '\0';
	return (chk_apply(r.line, a, b));
}
