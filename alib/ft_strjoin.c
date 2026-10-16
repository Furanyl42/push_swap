/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 18:59:41 by nide-wil          #+#    #+#             */
/*   Updated: 2026/09/01 19:15:32 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin(char const *s1, char const *s2)
{
	size_t	s1len;
	size_t	s2len;
	char	*joined;

	if (!s1 || !s2)
		return (0);
	s1len = ft_strlen(s1);
	s2len = ft_strlen(s2);
	joined = ft_calloc(s1len + s2len + 1, sizeof(char));
	if (!joined)
		return (0);
	ft_memcpy(joined, s1, s1len);
	ft_memcpy(joined + s1len, s2, s2len);
	joined[s1len + s2len] = '\0';
	return (joined);
}
