/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 15:34:48 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/16 15:35:13 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int get_stack_element(t_stack *s, int index)
{
    // Sécurité pour éviter de sortir des limites de la pile active
    if (index < 0 || index >= s->size)
    {
        // Tu peux gérer l'erreur selon tes besoins
        return (-1); 
    }
    
    // On part de head, on avance de 'index', et on boucle avec capacity
    return (s->tab[(s->head + index) % s->capacity]);
}
