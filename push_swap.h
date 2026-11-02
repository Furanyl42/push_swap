/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 12:00:00 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/02 12:00:00 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PUSH_SWAP_H
# define PUSH_SWAP_H

# include "libft.h"
# include <stdlib.h>
# include <unistd.h>

typedef struct s_counts
{
	int				sa;
	int				sb;
	int				ss;
	int				pa;
	int				pb;
	int				ra;
	int				rb;
	int				rr;
	int				rra;
	int				rrb;
	int				rrr;
}					t_counts;

typedef struct s_stack
{
	int				*tab;
	int				size;
	int				capacity;
	int				head;
	int				quiet;
	t_counts		*counts;
}					t_stack;

typedef enum e_strategy
{
	STRAT_ADAPTIVE,
	STRAT_SIMPLE,
	STRAT_MEDIUM,
	STRAT_COMPLEX
}					t_strategy;

typedef struct s_opts
{
	t_strategy		strategy;
	int				bench;
}					t_opts;

typedef struct s_range
{
	int				low;
	int				high;
}					t_range;

typedef struct s_geom
{
	int				div;
	int				fr;
	int				n;
}					t_geom;

int					get_stack_element(t_stack *s, int index);
int					has_duplicate(int *tab, int size, int value);
t_stack				*init_stack(int capacity, t_counts *counts);
void				free_stack(t_stack *s);

double				compute_disorder(t_stack *a);
int					is_sorted(t_stack *a);

int					get_cost(int index, int size);
void				init_counts(t_counts *counts);
int					total_ops(t_counts *counts);

int					is_space(char c);
int					token_count(char *s);
int					parse_one(char *s, int *pos, long *value);
int					count_numbers(int argc, char **argv);
int					fill_stack(t_stack *a, int argc, char **argv);
int					is_flag_token(char *arg);
int					parse_flags(int argc, char **argv, t_opts *opts);
t_stack				*parse_input(int argc, char **argv, t_opts *opts);

t_stack				*parse_checker(int argc, char **argv, t_counts *counts);
int					apply_op(char *line, t_stack *a, t_stack *b);
int					run_instructions(t_stack *a, t_stack *b);
int					checker_error(t_stack *a, t_stack *b);

void				put_pct(double disorder);
void				print_bench(t_counts *counts, double disorder,
						t_strategy requested, t_strategy resolved);

void				sa(t_stack *a);
void				sb(t_stack *b);
void				ss(t_stack *a, t_stack *b);
void				pa(t_stack *a, t_stack *b);
void				pb(t_stack *a, t_stack *b);
void				ra(t_stack *a);
void				rb(t_stack *b);
void				rr(t_stack *a, t_stack *b);
void				rra(t_stack *a);
void				rrb(t_stack *b);
void				rrr(t_stack *a, t_stack *b);

int					*build_sorted_copy(t_stack *a);
int					find_rank(int *sorted, int n, int value);

int					normalize_stack(t_stack *a);
void				complex_presort(t_stack *a, t_stack *b, int div, int fr);
int					complex_find(t_stack *s, int value);
int					complex_min_index(t_stack *a);
int					complex_target_index(t_stack *a, int value);
void				complex_align(t_stack *a);
void				complex_insert(t_stack *a, t_stack *b, int value);
int					complex_cost(int cost_a, int cost_b);
void				complex_pull(t_stack *a, t_stack *b, t_geom *geom);

t_strategy			resolve_strategy(t_strategy requested, double disorder);
void				sort_stacks(t_stack *a, t_stack *b, t_strategy strat);
void				sort_simple(t_stack *a, t_stack *b);
void				sort_medium(t_stack *a, t_stack *b);
void				sort_complex(t_stack *a, t_stack *b);

#endif
