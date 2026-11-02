/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checker_read.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 12:00:00 by nide-wil          #+#    #+#             */
/*   Updated: 2026/11/02 12:00:00 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static char	*grow_buf(char *buf, int *cap)
{
	char	*new;
	int		size;

	size = *cap;
	new = malloc(size * 2 + 1);
	if (!new)
	{
		free(buf);
		return (NULL);
	}
	ft_memcpy(new, buf, size);
	free(buf);
	*cap = size * 2;
	return (new);
}

static int	read_loop(int fd, char **buf, int *cap)
{
	int	len;
	int	r;

	len = 0;
	r = 1;
	while (r > 0)
	{
		r = read(fd, *buf + len, *cap - len);
		if (r < 0)
			return (-1);
		len += r;
		if (len == *cap)
		{
			*buf = grow_buf(*buf, cap);
			if (!*buf)
				return (-1);
		}
	}
	return (len);
}

static char	*read_all(int fd)
{
	char	*buf;
	int		cap;
	int		len;

	cap = 1024;
	buf = malloc(cap + 1);
	if (!buf)
		return (NULL);
	len = read_loop(fd, &buf, &cap);
	if (len < 0)
	{
		free(buf);
		return (NULL);
	}
	buf[len] = '\0';
	return (buf);
}

int	run_instructions(t_stack *a, t_stack *b)
{
	char	*buf;
	char	*line;
	char	*nl;

	buf = read_all(0);
	if (!buf)
		return (0);
	line = buf;
	while (*line)
	{
		nl = ft_strchr(line, '\n');
		if (nl)
			*nl = '\0';
		if (!apply_op(line, a, b))
		{
			free(buf);
			return (0);
		}
		if (!nl)
			break ;
		line = nl + 1;
	}
	free(buf);
	return (1);
}
