/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_basic_op.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 15:26:28 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/16 15:33:34 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"
#include <unistd.h>

void sa(t_stack *a)
{
    int temp;
    int idx1;
    int idx2;

    if (a->size < 2)
        return;
    idx1 = a->head;
    idx2 = (a->head + 1) % a->capacity;
    
    temp = a->tab[idx1];
    a->tab[idx1] = a->tab[idx2];
    a->tab[idx2] = temp;
    
    write(1, "sa\n", 3);
}

void ra(t_stack *a)
{
    if (a->size < 2)
        return;
    a->head = (a->head + 1) % a->capacity;
    write(1, "ra\n", 3);
}

void pb(t_stack *a, t_stack *b)
{
    int val;

    if (a->size == 0)
        return;

    val = a->tab[a->head];
    a->head = (a->head + 1) % a->capacity;
    a->size--;

    b->head = (b->head - 1 + b->capacity) % b->capacity;
    b->tab[b->head] = val;
    b->size++;

    write(1, "pb\n", 3);
}
