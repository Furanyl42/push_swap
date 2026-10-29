/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 14:24:02 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/29 17:55:59 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include "push_swap.h"
#include "../libft/libft.h"

int is_valid_number(char *str)
{
    int i;

    i = 0;
    if (str[i] == '-' || str[i] == '+')
        i++;
    if (!str[i])
        return (0);
    while (str[i])
    {
        if (str[i] < '0' || str[i] > '9')
            return (0);
        i++;
    }
    return (1);
}

long ft_atol(const char *str)
{
    long    res;
    int     sign;
    int     i;

    res = 0;
    sign = 1;
    i = 0;
    while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
        i++;
    if (str[i] == '-' || str[i] == '+')
    {
        if (str[i] == '-')
            sign = -1;
        i++;
    }
    while (str[i] >= '0' && str[i] <= '9')
    {
        res = res * 10 + (str[i] - '0');
        i++;
    }
    return (res * sign);
}

int has_duplicate(int *tab, int size, int val)
{
    int i;

    i = 0;
    while (i < size)
    {
        if (tab[i] == val)
            return (1);
        i++;
    }
    return (0);
}

void free_split(char **split)
{
    int i;

    if (!split)
        return;
    i = 0;
    while (split[i])
    {
        free(split[i]);
        i++;
    }
    free(split);
}

int parse_stack(t_stack *a, char **args, int is_split)
{
    int     i;
    long    num;
    int     count;

    count = 0;
    while (args[count])
        count++;

    a->capacity = count;
    a->size = count;
    a->head = 0;
    a->tab = malloc(sizeof(int) * count);
    if (!a->tab)
        return (0);

    i = 0;
    while (i < count)
    {
        if (!is_valid_number(args[i]))
            return (free(a->tab), 0);
        
        num = ft_atol(args[i]);
        if (num < -2147483648 || num > 2147483647)
            return (free(a->tab), 0);
        
        if (has_duplicate(a->tab, i, (int)num))
            return (free(a->tab), 0);
            
        a->tab[i] = (int)num;
        i++;
    }
    if (is_split)
        free_split(args);
    return (1);
}

t_stack *parse_input(int argc, char **argv)
{
    t_stack *a;
    char    **args;
    int     is_split;

    a = malloc(sizeof(t_stack));
    if (!a)
        return (NULL);

    // Cas où l'utilisateur passe une seule chaîne entre guillemets ex: "./push_swap "3 2 1""
    if (argc == 2)
    {
        args = ft_split(argv[1], ' '); // Nécessite ta fonction ft_split de la Libft
        is_split = 1;
    }
    else // Cas où l'utilisateur passe des arguments multiples ex: "./push_swap 3 2 1"
    {
        args = argv + 1;
        is_split = 0;
    }

    if (!args || !parse_stack(a, args, is_split))
    {
        free(a);
        return (NULL);
    }
    return (a);
}
