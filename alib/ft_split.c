/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 22:19:40 by nide-wil          #+#    #+#             */
/*   Updated: 2026/09/02 19:34:39 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static size_t	nbofstrs(char const *s, char c)
{
	int		inword;
	size_t	n;

	inword = 0;
	n = 0;
	while (*s)
	{
		if (*s != c && inword == 0)
		{
			inword = 1;
			n++;
		}
		else if (*s == c)
			inword = 0;
		s++;
	}
	return (n);
}

static void	free_split(char **split, size_t i)
{
	while (i > 0)
	{
		i--;
		free(split[i]);
	}
	free(split);
}

char	**ft_split(char const *s, char c)
{
	char	**split;
	size_t	i;
	size_t	len;

	if (!s)
		return (0);
	split = ft_calloc(nbofstrs(s, c) + 1, sizeof(char *));
	if (!split)
		return (0);
	i = 0;
	while (*s)
	{
		if (*s == c && ++s)
			continue ;
		len = 0;
		while (s[len] && s[len] != c)
			len++;
		split[i] = ft_substr(s, 0, len);
		if (!split[i])
			return (free_split(split, i), NULL);
		s += len;
		i++;
	}
	return (split);
}
