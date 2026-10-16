/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 22:52:19 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/15 12:56:49 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static size_t	getsize(int n)
{
	size_t	size;

	size = 0;
	if (n <= 0)
		size++;
	while (n)
	{
		n /= 10;
		size++;
	}
	return (size);
}

char	*ft_itoa(int n)
{
	long	v;
	size_t	len;
	char	*result;

	v = (long)n;
	len = getsize(n);
	result = ft_calloc(len + 1, sizeof(char));
	if (!result)
		return (0);
	result[len] = '\0';
	if (v == 0)
		result[0] = '0';
	if (v < 0)
	{
		v = -v;
		result[0] = '-';
	}
	while (v)
	{
		len--;
		result[len] = (v % 10) + '0';
		v /= 10;
	}
	return (result);
}
