/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cost.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 15:42:25 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/29 15:58:14 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int get_cost(int idx, int size)
{
    if (idx <= size / 2)
        return (idx);
    else
        return (idx - size);
}

static int absolute(int n)
{
    if (n < 0)
        return (-n);
    return (n);
}

int calculate_total_cost(int cost_a, int cost_b)
{
    int abs_a;
    int abs_b;

    abs_a = absolute(cost_a);
    abs_b = absolute(cost_b);
    
    // Si les deux coûts ont le même signe (rr ou rrr possibles)
    if ((cost_a > 0 && cost_b > 0) || (cost_a < 0 && cost_b < 0))
    {
        if (abs_a > abs_b)
            return (abs_a);
        return (abs_b);
    }
    // Si les signes sont opposés (mouvements indépendants)
    else
    {
        return (abs_a + abs_b);
    }
}
