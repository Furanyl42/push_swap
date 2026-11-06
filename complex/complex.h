/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   complex.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/04 12:12:28 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/05 12:00:00 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMPLEX_H
# define COMPLEX_H

# include "push_swap.h"

typedef struct s_range
{
	int	low;
	int	high;
}	t_range;

typedef struct s_geom
{
	int	n;
	int	div;
	int	fr;
	int	chunk_size;
}	t_geom;

void	complex_geometry(t_geom *geom);
void	complex_presort(t_stack *a, t_stack *b, t_geom *geom);
void	complex_pull(t_stack *a, t_stack *b, t_geom *geom);
void	complex_align(t_stack *a);

int		get_cost(int index, int size);
int		get_min_idx(t_stack *a);
int		get_target_idx(t_stack *a, int value);
int		combined_cost(int cost_a, int cost_b);
int		score_at(t_stack *a, t_stack *b, int i, int value);

int		complex_find(t_stack *b, int value);
void	rotate_ab(t_stack *a, t_stack *b, int cost_a, int cost_b);
void	complex_insert(t_stack *a, t_stack *b, int value);
int		best_in_range(t_stack *a, t_stack *b, t_range *range,
			int *best_val);

#endif
