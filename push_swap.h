/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 14:08:12 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/16 15:56:49 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

typedef struct s_stack
{
	int	*tab;
	int	size;
	int	capacity;
	int	head;
}	t_stack;

int get_stack_element(t_stack *s, int index);
void rrr(t_stack *a, t_stack *b);
void rrb(t_stack *b);
void rra(t_stack *a);
void rr(t_stack *a, t_stack *b);
void rb(t_stack *b);
void ra(t_stack *a);
void pa(t_stack *a, t_stack *b);
void pb(t_stack *a, t_stack *b);
void ss(t_stack *a, t_stack *b);
void sb(t_stack *b);
void sa(t_stack *a);
