/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ops.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 15:26:28 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/29 16:16:30 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"
#include <unistd.h>

void sa(t_stack *a)
{
    int temp;
    int i1, i2;

    if (a->size < 2)
        return;
    i1 = a->head;
    i2 = (a->head + 1) % a->capacity;
    temp = a->tab[i1];
    a->tab[i1] = a->tab[i2];
    a->tab[i2] = temp;
    write(1, "sa\n", 3);
}

void sb(t_stack *b)
{
    int temp;
    int i1, i2;

    if (b->size < 2)
        return;
    i1 = b->head;
    i2 = (b->head + 1) % b->capacity;
    temp = b->tab[i1];
    b->tab[i1] = b->tab[i2];
    b->tab[i2] = temp;
    write(1, "sb\n", 3);
}

void ss(t_stack *a, t_stack *b)
{
    int temp;
    int i1, i2;

    if (a->size >= 2)
    {
        i1 = a->head;
        i2 = (a->head + 1) % a->capacity;
        temp = a->tab[i1];
        a->tab[i1] = a->tab[i2];
        a->tab[i2] = temp;
    }
    if (b->size >= 2)
    {
        i1 = b->head;
        i2 = (b->head + 1) % b->capacity;
        temp = b->tab[i1];
        b->tab[i1] = b->tab[i2];
        b->tab[i2] = temp;
    }
    write(1, "ss\n", 3);
}

void pb(t_stack *a, t_stack *b)
{
    int val;

    if (a->size == 0)
        return;
    val = a->tab[a->head];
    a->head = (a->head + 1) % a->capacity;
    a->size--;

    b->head = (b->head - 1 + b->capacity) % b->capacity;
    b->tab[b->head] = val;
    b->size++;

    write(1, "pb\n", 3);
}

void pa(t_stack *a, t_stack *b)
{
    int val;

    if (b->size == 0)
        return;
    val = b->tab[b->head];
    b->head = (b->head + 1) % b->capacity;
    b->size--;

    a->head = (a->head - 1 + a->capacity) % a->capacity;
    a->tab[a->head] = val;
    a->size++;

    write(1, "pa\n", 3);
}

void ra(t_stack *a)
{
    int val;
    int tail;

    if (a->size < 2)
        return;
    val = a->tab[a->head];
    tail = (a->head + a->size) % a->capacity;
    a->tab[tail] = val;
    a->head = (a->head + 1) % a->capacity;
    write(1, "ra\n", 3);
}

void rb(t_stack *b)
{
    int val;
    int tail;

    if (b->size < 2)
        return;
    val = b->tab[b->head];
    tail = (b->head + b->size) % b->capacity;
    b->tab[tail] = val;
    b->head = (b->head + 1) % b->capacity;
    write(1, "rb\n", 3);
}

void rra(t_stack *a)
{
    int val;
    int new_head;

    if (a->size < 2)
        return;
    val = a->tab[(a->head + a->size - 1) % a->capacity];
    new_head = (a->head - 1 + a->capacity) % a->capacity;
    a->tab[new_head] = val;
    a->head = new_head;
    write(1, "rra\n", 4);
}

void rrb(t_stack *b)
{
    int val;
    int new_head;

    if (b->size < 2)
        return;
    val = b->tab[(b->head + b->size - 1) % b->capacity];
    new_head = (b->head - 1 + b->capacity) % b->capacity;
    b->tab[new_head] = val;
    b->head = new_head;
    write(1, "rrb\n", 4);
}

void rr(t_stack *a, t_stack *b)
{
    int val;
    int tail;

    if (a->size >= 2)
    {
        val = a->tab[a->head];
        tail = (a->head + a->size) % a->capacity;
        a->tab[tail] = val;
        a->head = (a->head + 1) % a->capacity;
    }
    if (b->size >= 2)
    {
        val = b->tab[b->head];
        tail = (b->head + b->size) % b->capacity;
        b->tab[tail] = val;
        b->head = (b->head + 1) % b->capacity;
    }
    write(1, "rr\n", 3);
}

void rrr(t_stack *a, t_stack *b)
{
    int val;
    int new_head;

    if (a->size >= 2)
    {
        val = a->tab[(a->head + a->size - 1) % a->capacity];
        new_head = (a->head - 1 + a->capacity) % a->capacity;
        a->tab[new_head] = val;
        a->head = new_head;
    }
    if (b->size >= 2)
    {
        val = b->tab[(b->head + b->size - 1) % b->capacity];
        new_head = (b->head - 1 + b->capacity) % b->capacity;
        b->tab[new_head] = val;
        b->head = new_head;
    }
    write(1, "rrr\n", 4);
}
