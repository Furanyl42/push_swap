/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 18:42:53 by nide-wil          #+#    #+#             */
/*   Updated: 2026/09/01 18:59:27 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	size_t	slen;
	char	*subs;

	if (!s)
		return (0);
	slen = ft_strlen(s);
	if (start >= slen)
		len = 0;
	else if (len > slen - start)
		len = slen - start;
	subs = ft_calloc(len + 1, sizeof(char));
	if (!subs)
		return (0);
	ft_memcpy(subs, s + start, len);
	subs[len] = '\0';
	return (subs);
}
