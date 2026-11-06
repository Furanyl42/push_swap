/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checker_bonus.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesmet <adesmet@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/05 13:00:00 by adesmet           #+#    #+#             */
/*   Updated: 2026/11/05 13:00:00 by adesmet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHECKER_BONUS_H
# define CHECKER_BONUS_H

# include "push_swap.h"

typedef struct s_reader
{
	char	buf[4096];
	char	line[8];
	int		len;
	t_stack	*a;
	t_stack	*b;
}			t_reader;

t_stack	*chk_parse(int ac, char **av);
int		chk_execute(t_stack *a, t_stack *b);
int		chk_apply(const char *line, t_stack *a, t_stack *b);
int		chk_streq(const char *a, const char *b);
void	chk_swap(t_stack *s);
void	chk_rotate(t_stack *s);
void	chk_reverse(t_stack *s);
void	chk_push(t_stack *dst, t_stack *src);
int		chk_sorted(t_stack *a);

#endif
