/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adesmet <adesmet@student.42belgium.be>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 14:08:12 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/04 18:59:51 by adesmet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PUSH_SWAP_H
# define PUSH_SWAP_H

# include "libft.h"
# include <limits.h>
# include <stdlib.h>
# include <unistd.h>

typedef struct s_count
{
	int			sa;
	int			sb;
	int			ss;
	int			pa;
	int			pb;
	int			ra;
	int			rb;
	int			rr;
	int			rra;
	int			rrb;
	int			rrr;
}				t_count;

typedef struct s_stack
{
	int			*tab;
	int			*idx;
	int			size;
	int			capacity;
	int			head;
	t_count		*count;
}				t_stack;

typedef struct s_cost
{
	int			a_idx;
	int			b_idx;
	int			cost_a;
	int			cost_b;
	int			total_cost;
}				t_cost;

typedef enum e_strategy
{
	STRAT_ADAPTIVE,
	STRAT_SIMPLE,
	STRAT_MEDIUM,
	STRAT_COMPLEX
}				t_strategy;

typedef struct s_opts
{
	t_strategy	strategy;
	int			bench;
}				t_opts;

int				token_count(const char *s);
int				count_numbers(int argc, char **argv);
int				streq(const char *a, const char *b);
int				apply_flag(const char *arg, t_opts *opts);
int				is_flag_token(const char *arg);
int				parse_flags(int argc, char **argv, t_opts *opts);
int				is_space(char c);
int				read_sign(const char **s);
int				parse_int(const char **s, int *out);
int				has_duplicate(int *tab, int size, int val);
int				fill_arg(t_stack *a, const char *s, int *idx);
int				fill_stack(t_stack *a, int argc, char **argv);
int				get_stack_element(t_stack *s, int index);
t_count			*init_count(void);
t_stack			*init_stack(int capacity);
t_stack			*parse_input(int argc, char **argv, t_opts *opts);
void			free_stack(t_stack *s);

void			rrr(t_stack *a, t_stack *b);
void			rrb(t_stack *b);
void			rra(t_stack *a);
void			rr(t_stack *a, t_stack *b);
void			rb(t_stack *b);
void			ra(t_stack *a);
void			pa(t_stack *a, t_stack *b);
void			pb(t_stack *a, t_stack *b);
void			ss(t_stack *a, t_stack *b);
void			sb(t_stack *b);
void			sa(t_stack *a);

int				ft_min_idx(t_stack *a);
void			ft_min_to_top(t_stack *a);
void			simple_sort(t_stack *a, t_stack *b);

int				floor_sqrt(int n);
int				get_max_pos(t_stack *b);
int				get_chunk_size(int size, int pct);
void			ft_swap(int *a, int *b);
void			insertion_sort(int *tab, int size);
void			index_stack(t_stack *a);
void			push_chunks(t_stack *a, t_stack *b, int size);
void			push_to_a(t_stack *a, t_stack *b);
void			medium_sort(t_stack *a, t_stack *b);

void			complex_sort(t_stack *a, t_stack *b);

double			compute_disorder(t_stack a);
t_strategy		strategy_from_disorder(double disorder);

t_stack			*generate_random_stack(int n, int min, int max);
void			print_stacks(t_stack *a, t_stack *b);
void			merge_counts(t_count *dst, t_count *src);
void			print_operation_counts(t_count *count);
int				get_total_operations(t_count *count);
void			print_bench(t_strategy strategy, double disorder,
					t_count *count);

#endif