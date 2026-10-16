/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 16:55:36 by nide-wil          #+#    #+#             */
/*   Updated: 2026/10/16 11:16:46 by nide-wil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static char	*ft_read_and_join(int fd, char *remainder)
{
	char	buf[BUFFER_SIZE + 1];
	int		bytes_read;
	char	*tmp;

	bytes_read = 1;
	while (bytes_read > 0 && !ft_strchr(remainder, '\n'))
	{
		bytes_read = read(fd, buf, BUFFER_SIZE);
		if (bytes_read < 0)
		{
			free(remainder);
			return (NULL);
		}
		buf[bytes_read] = '\0';
		tmp = ft_strjoin(remainder, buf);
		free(remainder);
		remainder = tmp;
	}
	return (remainder);
}

static char	*ft_set_line(char *remainder)
{
	int		i;
	char	*line;

	i = 0;
	if (!remainder[i])
		return (NULL);
	while (remainder[i] && remainder[i] != '\n')
		i++;
	line = ft_substr(remainder, 0, i + (remainder[i] == '\n'));
	return (line);
}

static char	*ft_update_remainder(char *remainder)
{
	int		i;
	char	*new_rem;

	i = 0;
	while (remainder[i] && remainder[i] != '\n')
		i++;
	if (!remainder[i])
	{
		free(remainder);
		return (NULL);
	}
	new_rem = ft_strdup(&remainder[i + 1]);
	free(remainder);
	return (new_rem);
}

char	*get_next_line(int fd)
{
	static char	*remainder;
	char		*line;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	remainder = ft_read_and_join(fd, remainder);
	if (!remainder)
		return (NULL);
	line = ft_set_line(remainder);
	remainder = ft_update_remainder(remainder);
	return (line);
}
