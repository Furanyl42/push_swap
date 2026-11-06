/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesmet <adesmet@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 13:17:45 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/05 12:00:00 by adesmet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf/ft_printf.h"
#include "test.h"

void	print_stacks(t_stack *a, t_stack *b)
{
	int	max_size;
	int	i;

	max_size = a->size;
	if (b->size > max_size)
		max_size = b->size;
	ft_printf("\n┌─────────┬─────────┐\n");
	ft_printf("│ Stack A │ Stack B │\n");
	ft_printf("├─────────┼─────────┤\n");
	i = -1;
	while (++i < max_size)
		print_row(a, b, i);
	ft_printf("└─────────┴─────────┘\n");
	ft_printf("  (size: %d)   (size: %d)\n\n", a->size, b->size);
}

void	print_operation_counts(t_count *count)
{
	if (!count)
	{
		ft_printf("\n[Count Error] Pointer is NULL\n");
		return ;
	}
	ft_printf("\n┌─────────────────────────────┐\n");
	ft_printf("│      OPERATION COUNTS       │\n");
	ft_printf("├─────────────────────────────┤\n");
	print_first(count);
	print_second(count);
	ft_printf("│  TOTAL: %-19d │\n", get_total_operations(count));
	ft_printf("└─────────────────────────────┘\n");
}

int	main(void)
{
	t_stack	*a;
	t_stack	*b;

	a = generate_random_stack(500, 0, 100000);
	if (!a)
		return (1);
	b = init_stack(a->capacity);
	if (!b)
		return (free_stack(a), 1);
	print_stacks(a, b);
	medium_sort(a, b);
	print_stacks(a, b);
	merge_counts(a->count, b->count);
	print_operation_counts(a->count);
	free_stack(a);
	free_stack(b);
	return (0);
}
