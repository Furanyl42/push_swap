/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   phase2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 15:31:09 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/29 16:37:40 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../push_swap.h"
#include <limits.h>

int find_min_index(t_stack *a)
{
    int min_val;
    int min_idx;
    int i;

    if (a->size <= 0)
        return (0);
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

int find_target_in_a(t_stack *a, int elem_b)
{
    int i;
    int target_idx;
    int closest_bigger;
    int current_val;

    target_idx = -1;
    closest_bigger = INT_MAX;
    i = 0;
    while (i < a->size)
    {
        current_val = get_stack_element(a, i);
        // On cherche le plus petit nombre de a qui est supérieur à elem_b
        if (current_val > elem_b && current_val < closest_bigger)
        {
            closest_bigger = current_val;
            target_idx = i;
        }
        i++;
    }
    // Si elem_b est plus grand que tous les éléments de a,
    // sa cible est le minimum de a
    if (target_idx == -1)
        target_idx = find_min_index(a);

    return (target_idx);
}

int move_cheapest_b_to_a(t_stack *a, t_stack *b)
{
    int     i;
    int     min_cost;
    t_cost  best_move;
    t_cost  current_move;

    min_cost = INT_MAX;
    i = 0;
    while (i < b->size)
    {
        current_move.b_idx = i;
        current_move.a_idx = find_target_in_a(a, get_stack_element(b, i));
        
        current_move.cost_a = get_cost(current_move.a_idx, a->size);
        current_move.cost_b = get_cost(current_move.b_idx, b->size);
        current_move.total_cost = calculate_total_cost(current_move.cost_a, current_move.cost_b);

        if (current_move.total_cost < min_cost)
        {
            min_cost = current_move.total_cost;
            best_move = current_move;
        }
        i++;
    }
    return (execute_optimal_moves(a, b, best_move, 2));
}

int	 final_alignment(t_stack *a)
{
    int min_idx;
    int cost;
	int count;

	count = 0;
    min_idx = find_min_index(a);
    cost = get_cost(min_idx, a->size);

    while (cost != 0)
    {
        if (cost > 0)
        {
            ra(a);
            cost--;
			count++;
        }
        else
        {
            rra(a);
            cost++;
			count++;
        }
    }
	return (count);
}
