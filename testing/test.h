/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesmet <adesmet@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/05 12:00:00 by adesmet           #+#    #+#             */
/*   Updated: 2026/11/05 12:00:00 by adesmet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TEST_H
# define TEST_H

# include "push_swap.h"

void	print_cell(t_stack *s, int i);
void	print_row(t_stack *a, t_stack *b, int i);
void	print_op(const char *name, int value);
void	print_first(t_count *c);
void	print_second(t_count *c);

#endif
