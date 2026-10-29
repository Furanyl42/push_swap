/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 16:47:08 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/29 16:19:07 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"
#include <stdio.h>

int main(void)
{

	t_stack *a;
	t_stack *b;

	double	disorder_score;

	// if (ac < 2)
	// 	return (0);
	a = generate_random_stack(100, 0, 500);
	b = init_stack(100);
	disorder_score = compute_disorder(*a);

	printf("--- STATS --- \n");
	printf("Score de désordre initial : %.4f (%.1f%%)\n", disorder_score, disorder_score * 100.0);
	printf("-------------\n\n");
	print_stacks(a, b);
	sort_stacks(a, b);
	print_stacks(a, b);
	free_stack(a);
	free_stack(b);
	return (0);
}


