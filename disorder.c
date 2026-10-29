/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   disorder.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 14:39:38 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/29 14:40:50 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

double compute_disorder(t_stack a)
{
    int     i;
    int     j;
    long    mistakes;
    long    total_pairs;

    if (a.size <= 1)
        return (0.0);
    mistakes = 0;
    total_pairs = 0;
    i = 0;
    while (i < a.size)
    {
        j = i + 1;
        while (j < a.size)
        {
            total_pairs++;
            if (get_stack_element(&a, i) > get_stack_element(&a, j))
                mistakes++;
            j++;
        }
        i++;
    }
    if (total_pairs == 0)
        return (0.0);
    return ((double)mistakes / (double)total_pairs);
}
