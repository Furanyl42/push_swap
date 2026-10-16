/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/31 21:48:14 by nide-wil          #+#    #+#             */
/*   Updated: 2026/08/31 21:52:54 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t n, size_t size)
{
	void	*p;
	size_t	total;

	if (n != 0 && size > (size_t)-1 / n)
		return (0);
	total = n * size;
	p = malloc(total);
	if (!p)
		return (0);
	ft_bzero(p, total);
	return (p);
}
