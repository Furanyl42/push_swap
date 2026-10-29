/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_dispatcher.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 15:44:49 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/29 16:32:58 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int sort_stacks(t_stack *a, t_stack *b)
{
    double disorder;
	int count;

	count = 0;

    // 1. Mesure obligatoire du désordre (requis par le sujet)
    disorder = compute_disorder(*a);
    
    // Si la pile est déjà triée (désordre == 0.0), on ne fait rien
    if (disorder == 0.0)
        return (0);

    // 2. Dispatch selon la taille de la pile a
    if (a->size == 2)
    {
        if (get_stack_element(a, 0) > get_stack_element(a, 1))
		{
			sa(a);
			count++;
		}
    }
    else if (a->size == 3)
        count += sort_three(a);
    else if (a->size <= 5)
    {
        // Ta fonction pour trier 4 ou 5 nombres
        count += sort_five(a, b); 
    }
    else
    {
        // Le grand chelem : l'algorithme du Turc pour > 5 nombres
        count += turk_algorithm(a, b);
    }
	return (count);
}
