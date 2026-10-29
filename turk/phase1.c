/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   phase1.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 15:25:51 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/29 15:28:00 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../push_swap.h"
#include <limits.h>

int get_max_index(t_stack *b)
{
    int max_val;
    int max_idx;
    int i;

    if (b->size <= 0)
        return (0);
    max_val = get_stack_element(b, 0);
    max_idx = 0;
    i = 1;
    while (i < b->size)
    {
        if (get_stack_element(b, i) > max_val)
        {
            max_val = get_stack_element(b, i);
            max_idx = i;
        }
        i++;
    }
    return (max_idx);
}

int find_target_in_b(t_stack *b, int elem_a)
{
    int i;
    int target_idx;
    int closest_smaller;
    int current_val;

    target_idx = -1;
    closest_smaller = INT_MIN;
    i = 0;
    while (i < b->size)
    {
        current_val = get_stack_element(b, i);
        
        if (current_val < elem_a && current_val > closest_smaller)
        {
            closest_smaller = current_val;
            target_idx = i;
        }
        i++;
    }
    
    if (target_idx == -1)
        target_idx = get_max_index(b);

    return (target_idx);
}
