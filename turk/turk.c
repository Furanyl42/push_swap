/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   turk.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 15:32:28 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/29 16:53:12 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../push_swap.h"

int turk_algorithm(t_stack *a, t_stack *b)
{
	int count;

	count = 0;
    if (a->size > 3)
	{	
		count++;
        pb(a, b);
	}
    if (a->size > 3)
	{
		count++;
		pb(a, b);
	}    

    while (a->size > 3)
    {
        count += move_cheapest_a_to_b(a, b);
    }

    count += sort_three(a);

    while (b->size > 0)
    {
        count += move_cheapest_b_to_a(a, b);
    }

    count += final_alignment(a);
	return (count);
}
