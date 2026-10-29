/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_dispatcher.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 15:44:49 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/29 15:47:07 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void sort_stacks(t_stack *a, t_stack *b)
{
    double disorder;

    // 1. Mesure obligatoire du désordre (requis par le sujet)
    disorder = compute_disorder(*a);
    
    // Si la pile est déjà triée (désordre == 0.0), on ne fait rien
    if (disorder == 0.0)
        return;

    // 2. Dispatch selon la taille de la pile a
    if (a->size == 2)
    {
        if (get_stack_element(a, 0) > get_stack_element(a, 1))
            sa(a);
    }
    else if (a->size == 3)
    {
        sort_three(a);
    }
    else if (a->size <= 5)
    {
        // Ta fonction pour trier 4 ou 5 nombres
        sort_five(a, b); 
    }
    else
    {
        // Le grand chelem : l'algorithme du Turc pour > 5 nombres
        turk_algorithm(a, b);
    }
}
