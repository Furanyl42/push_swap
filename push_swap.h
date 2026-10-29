/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 14:08:12 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/29 15:47:44 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

typedef struct s_stack
{
	int	*tab;
	int	size;
	int	capacity;
	int	head;
}	t_stack;

typedef struct s_cost
{
    int a_idx;
    int b_idx;
    int cost_a;
    int cost_b;
    int total_cost;
}   t_cost;

int get_stack_element(t_stack *s, int index);

void rrr(t_stack *a, t_stack *b);
void rrb(t_stack *b);
void rra(t_stack *a);
void rr(t_stack *a, t_stack *b);
void rb(t_stack *b);
void ra(t_stack *a);
void pa(t_stack *a, t_stack *b);
void pb(t_stack *a, t_stack *b);
void ss(t_stack *a, t_stack *b);
void sb(t_stack *b);
void sa(t_stack *a);

void sort_three(t_stack *a);
void sort_five(t_stack *a, t_stack *b);

void execute_optimal_moves(t_stack *a, t_stack *b, t_cost move, int direction);

t_stack *generate_random_stack(int n, int min, int max);
t_stack *init_stack(int capacity);
void print_stacks(t_stack *a, t_stack *b);
void free_stack(t_stack *s);

double compute_disorder(t_stack a);

int     get_cost(int idx, int size);
int     calculate_total_cost(int cost_a, int cost_b);

int     get_max_index(t_stack *b);
int     find_min_index(t_stack *a);
int     find_target_in_b(t_stack *b, int elem_a);
int     find_target_in_a(t_stack *a, int elem_b);

void    turk_algorithm(t_stack *a, t_stack *b);
void    move_cheapest_a_to_b(t_stack *a, t_stack *b);
void    move_cheapest_b_to_a(t_stack *a, t_stack *b);
void    final_alignment(t_stack *a);

void sort_stacks(t_stack *a, t_stack *b);
