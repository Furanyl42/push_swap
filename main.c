/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 16:47:08 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/19 17:10:07 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"
#include <stdlib.h>
void print_stacks(t_stack a, t_stack b);

int main(void)
{
	t_stack a;
	t_stack b;

	a.capacity = 3;
	a.size = 3;
	a.head = 0;
	a.tab = malloc(sizeof(int) * 3);
	a.tab[0] = 2;
	a.tab[1] = 1;
	a.tab[2] = 5;

	b.capacity = 3;
	b.size = 3;
	b.head = 0;
	b.tab = malloc(sizeof(int) * 3);
	b.tab[0] = 8;
	b.tab[1] = 9;
	b.tab[2] = 1;


	print_stacks(a, b);
	free(a.tab);
	free(b.tab);
}


