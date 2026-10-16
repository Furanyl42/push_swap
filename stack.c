/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stack.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 15:34:48 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/16 15:52:26 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int get_stack_element(t_stack *s, int index)
{
    if (index < 0 || index >= s->size)
        return (-1); 
    
    return (s->tab[(s->head + index) % s->capacity]);
}
