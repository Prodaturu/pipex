/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sprodatu <sprodatu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 07:49:09 by sprodatu          #+#    #+#             */
/*   Updated: 2024/04/25 08:36:10 by sprodatu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

char	*ft_strjoin(const char *s1, const char *s2)
{
	char	*joint_string;
	size_t	total_length;

	total_length = (ft_strlen(s1) + ft_strlen(s2));
	joint_string = (char *)malloc(total_length + 1);
	if (joint_string == NULL)
		return (NULL);
	ft_memcpy(joint_string, s1, ft_strlen(s1));
	ft_memcpy(joint_string + ft_strlen(s1), s2, ft_strlen(s2));
	joint_string[total_length] = '\0';
	return (joint_string);
}
