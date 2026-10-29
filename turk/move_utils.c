/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   move_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 15:36:20 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/29 16:20:42 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../push_swap.h"

void execute_optimal_moves(t_stack *a, t_stack *b, t_cost move, int direction)
{
    // 1. Mouvements combinés vers le haut (rr)
    while (move.cost_a > 0 && move.cost_b > 0)
    {
        rr(a, b);
        move.cost_a--;
        move.cost_b--;
    }

    // 2. Mouvements combinés vers le bas (rrr)
    while (move.cost_a < 0 && move.cost_b < 0)
    {
        rrr(a, b);
        move.cost_a++;
        move.cost_b++;
    }

    // 3. Rotations restantes pour la pile A (ra ou rra)
    while (move.cost_a > 0)
    {
        ra(a);
        move.cost_a--;
    }
    while (move.cost_a < 0)
    {
        rra(a);
        move.cost_a++;
    }

    // 4. Rotations restantes pour la pile B (rb ou rrb)
    while (move.cost_b > 0)
    {
        rb(b);
        move.cost_b--;
    }
    while (move.cost_b < 0)
    {
        rrb(b);
        move.cost_b++;
    }

    // 5. Action finale : on pousse l'élément dans la bonne pile
    if (direction == 1) // Phase 1 : de a vers b
        pb(a, b);
    else                // Phase 2 : de b vers a
        pa(a, b);
}
