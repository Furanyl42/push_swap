/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stack.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 15:34:48 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/29 16:04:06 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"
#include <stdio.h>
#include <stdlib.h>

int get_stack_element(t_stack *s, int index)
{
    if (index < 0 || index >= s->size)
        return (0); // Sécurité propre sans polluer les nombres négatifs
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

t_stack *init_stack(int capacity)
{
    t_stack *s;

    s = malloc(sizeof(t_stack));
    if (!s)
        return (NULL);

    s->capacity = capacity;
    s->size = 0;
    s->head = 0;

    s->tab = malloc(sizeof(int) * capacity);
    if (!s->tab)
    {
        free(s);
        return (NULL);
    }

    return (s);
}

void free_stack(t_stack *s)
{
    if (!s)
        return;
    if (s->tab)
    {
        free(s->tab);
        s->tab = NULL;
    }
    free(s);
}
