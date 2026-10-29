/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 16:47:08 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/29 12:14:42 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/libft.h"
#include "push_swap.h"
#include <stdlib.h>

int main(void)
{
	t_stack *a;
	t_stack b;

	a = generate_random_stack(25, 0, 500);
	if (!a)
		return (1);

	ft_bzero(&b, sizeof(t_stack));
	b.capacity = 100;
	b.tab = malloc(sizeof(int) * b.capacity);
	if (!b.tab)
	{
		free(a->tab);
		free(a);
		return (1);
	}
	b.size = 0;
	b.head = 0;
	print_stacks(a, &b);
	free(a->tab);
	free(a);
	free(b.tab);
	return (0);
}


