/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sprodatu <sprodatu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 06:09:57 by sprodatu          #+#    #+#             */
/*   Updated: 2024/05/03 06:10:00 by sprodatu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	while (*s != '\0')
	{
		i++;
		s++;
	}
	return (i);
}

// char	**parse_arguments(char *cmd)
// {
// 	int		num_args;
// 	char 	**args;
// }

/**
 * Finds the command in the given path.
 *
 * @param cmd The command to find.
 * @param path_split An array of paths to search for the command.
 * @return The full path of the command if found, NULL otherwise.
 */

char	*find_cmd_in_path(char *cmd, char **path_split)
{
	char	*cmd_path;
	char	*tmp;
	int		i;

	i = 0;
	while (path_split[i] != NULL)
	{
		tmp = ft_strjoin(path_split[i], "/");
		if (tmp == NULL)
			return (NULL);
		cmd_path = ft_strjoin(tmp, cmd);
		if (cmd_path == NULL)
			return (NULL);
		free(tmp);
		if (access(cmd_path, F_OK | X_OK) == 0)
			return (cmd_path);
		free(cmd_path);
		i++;
	}
	return (NULL);
}

/**
 * Finds the path of a command in the environment variable PATH.
 *
 * @param cmd Command to find the path for.
 * @param envp Environment variable array.
 * @return The path of the command if found, NULL otherwise.
 */

char	*find_cmd_path(char *cmd, char **envp)
{
	char	*path;
	char	**path_split;
	char	*cmd_path;
	int		i;

	i = 0;
	path = NULL;
	path_split = NULL;
	while (envp[i] != NULL)
	{
		if (ft_strncmp(envp[i], "PATH=", 5) == 0)
		{
			path = ft_strdup(envp[i] + 5);
			if (path == NULL)
				return (NULL);
			break ;
		}
		i++;
	}
	path_split = ft_split(path, ':');
	if (path_split == NULL)
		return (free(path), NULL);
	cmd_path = find_cmd_in_path(cmd, path_split);
	ft_free_split(path_split);
	return (cmd_path);
}

/**
 * Executes a command with the given arguments and environment variables.
 *
 * @param cmd The command to execute.
 * @param envp The array of environment variables.
 * @return void
 * 
 * Find the path of the command
 * Split the command into arguments
 * Execute the command
 * 
 * Details:
 * - Find the path of the command using find_cmd_path
 * - Split the command into arguments using ft_split
 * - Execute the command using execve
 * - Print an error message if the command fails
 * - Free the allocated memory
 * - Exit the program
 */

void	exec_cmd(char *cmd, char **envp)
{
	char	*cmd_path;
	char	**cmd_args;

	cmd_args = ft_split(cmd, ' ');
	if (cmd_args == NULL)
	{
		perror("Error: Malloc failed\n");
		exit(EXIT_FAILURE);
	}
	cmd_path = find_cmd_path(cmd_args[0], envp);
	if (cmd_path == NULL)
	{
		perror("Error: Command not found\n");
		ft_free_split(cmd_args);
		exit(EXIT_FAILURE);
	}
	if (execve(cmd_path, cmd_args, envp) == -1)
	{
		perror("Error: Execve failed\n");
		free(cmd_path);
		ft_free_split(cmd_args);
		exit(EXIT_FAILURE);
	}
	free(cmd_path);
	ft_free_split(cmd_args);
}
