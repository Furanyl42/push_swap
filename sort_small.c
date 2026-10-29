/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_small.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 15:51:23 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/29 15:57:49 by nide-wil         ###   ########.fr       */
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

int get_min_index(t_stack *a)
{
    int min_val;
    int min_idx;
    int i;

    min_val = get_stack_element(a, 0);
    min_idx = 0;
    i = 1;
    while (i < a->size)
    {
        if (get_stack_element(a, i) < min_val)
        {
            min_val = get_stack_element(a, i);
            min_idx = i;
        }
        i++;
    }
    return (min_idx);
}

void sort_five(t_stack *a, t_stack *b)
{
    int min_idx;

    while (a->size > 3)
    {
        min_idx = get_min_index(a);
        if (min_idx <= a->size / 2)
        {
            while (get_min_index(a) != 0)
                ra(a);
        }
        else
        {
            while (get_min_index(a) != 0)
                rra(a);
        }
        pb(a, b);
    }
    sort_three(a);
    while (b->size > 0)
        pa(a, b);
}
