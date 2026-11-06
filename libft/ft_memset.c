/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/30 20:11:56 by nide-wil          #+#    #+#             */
/*   Updated: 2026/09/02 19:18:06 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memset(void *ptr, int value, size_t num)
{
	size_t			i;
	unsigned char	*p;

	i = 0;
	p = (unsigned char *)ptr;
	while (i < num)
		p[i++] = (unsigned char)value;
	return (ptr);
}
