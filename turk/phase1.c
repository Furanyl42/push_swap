/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   phase1.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 15:25:51 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/29 16:24:05 by nide-wil         ###   ########.fr       */
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
        // On cherche le plus grand nombre de b qui est inférieur à elem_a
        if (current_val < elem_a && current_val > closest_smaller)
        {
            closest_smaller = current_val;
            target_idx = i;
        }
        i++;
    }
    // Si elem_a est plus petit que tous les éléments de b,
    // sa cible est le maximum de b (pour se placer tout en bas)
    if (target_idx == -1)
        target_idx = get_max_index(b);

    return (target_idx);
}

int move_cheapest_a_to_b(t_stack *a, t_stack *b)
{
    int     i;
    int     min_cost;
    t_cost  best_move;
    t_cost  current_move;

    min_cost = INT_MAX;
    i = 0;
    while (i < a->size)
    {
        current_move.a_idx = i;
        current_move.b_idx = find_target_in_b(b, get_stack_element(a, i));
        
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
    return (execute_optimal_moves(a, b, best_move, 1));
}
