/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   turk.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 15:32:28 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/29 16:26:40 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../push_swap.h"

int turk_algorithm(t_stack *a, t_stack *b)
{
	int count;

	count = 0;
    // Étape 1 : Amorçage de la pile b (on pousse 2 éléments directement)
    if (a->size > 3)
        pb(a, b);
    if (a->size > 3)
        pb(a, b);

    // Étape 2 : Phase 1 - Déversement stratégique de a vers b jusqu'à ce qu'il reste 3 éléments
    while (a->size > 3)
    {
        count += move_cheapest_a_to_b(a, b);
    }

    // Étape 3 : Phase 2 - Trier les 3 derniers éléments restants dans a
    sort_three(a);

    // Étape 4 : Phase 3 - Retour trié de b vers a
    while (b->size > 0)
    {
        count += move_cheapest_b_to_a(a, b);
    }

    // Étape 5 : Phase 4 - Alignement final (amener le minimum de a tout en haut)
    count += final_alignment(a);
	return (count);
}
