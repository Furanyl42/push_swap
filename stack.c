/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stack.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 15:34:48 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/29 12:11:42 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"
#include <stdio.h>

int get_stack_element(t_stack *s, int index)
{
    if (index < 0 || index >= s->size)
        return (-1); 
    
    return (s->tab[(s->head + index) % s->capacity]);
}

void print_stacks(t_stack *a, t_stack *b)
{
    int max_size;
    int i;
    int has_a, has_b;

    max_size = (a->size > b->size) ? a->size : b->size;

    printf("\n┌─────────┬─────────┐\n");
    printf("│ Stack A │ Stack B │\n");
    printf("├─────────┼─────────┤\n");

    i = 0;
    while (i < max_size)
    {
        has_a = (i < a->size);
        has_b = (i < b->size);

        printf("│ ");
        if (has_a)
            printf("%7d", get_stack_element(a, i));
        else
            printf("       ");

        printf(" │ ");
        if (has_b)
            printf("%7d", get_stack_element(b, i));
        else
            printf("       ");

        printf(" │\n");
        i++;
    }
    printf("└─────────┴─────────┘\n");
    printf("  (size: %d)   (size: %d)\n\n", a->size, b->size);
}
