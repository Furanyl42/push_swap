/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   generator.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 11:23:29 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/29 11:25:03 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "push_swap.h"

t_stack *generate_random_stack(int n, int min, int max)
{
    t_stack *stack;
    int range;
    int *all_nums;
    int i, j, temp;

    if (max - min + 1 < n || n <= 0)
    {
        fprintf(stderr, "Erreur: Plage trop petite pour générer %d nombres uniques.\n", n);
        return (NULL);
    }

    range = max - min + 1;
    all_nums = malloc(sizeof(int) * range);
    if (!all_nums)
        return (NULL);

    i = 0;
    while (i < range)
    {
        all_nums[i] = min + i;
        i++;
    }

    srand(time(NULL));
    i = range - 1;
    while (i > 0)
    {
        j = rand() % (i + 1);
        temp = all_nums[i];
        all_nums[i] = all_nums[j];
        all_nums[j] = temp;
        i--;
    }

    stack = malloc(sizeof(t_stack));
    if (!stack)
    {
        free(all_nums);
        return (NULL);
    }
    
    stack->capacity = n;
    stack->size = n;
    stack->head = 0;
    stack->tab = malloc(sizeof(int) * n);
    if (!stack->tab)
    {
        free(all_nums);
        free(stack);
        return (NULL);
    }

    i = 0;
    while (i < n)
    {
        stack->tab[i] = all_nums[i];
        i++;
    }

    free(all_nums);
    return (stack);
}
