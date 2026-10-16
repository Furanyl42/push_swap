/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 15:51:23 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/16 15:56:09 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void sort_three(t_stack *a)
{
    int n0 = get_stack_element(a, 0);
    int n1 = get_stack_element(a, 1);
    int n2 = get_stack_element(a, 2);

    if (n0 > n1 && n0 > n2)
        ra(a);
    else if (n1 > n0 && n1 > n2)
        rra(a);

    n0 = get_stack_element(a, 0);
    n1 = get_stack_element(a, 1);
    if (n0 > n1)
        sa(a);
}
