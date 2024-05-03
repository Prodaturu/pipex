/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sprodatu <sprodatu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 16:22:52 by sprodatu          #+#    #+#             */
/*   Updated: 2024/05/02 14:43:00 by sprodatu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

// prints string to given file descriptor

void	ft_putstr_fd(char *s, int fd)
{
	if (s != NULL)
	{
		while (*s)
		{
			write(fd, s, 1);
			s++;
		}
	}
}

/**
 * Checks if specified files exist and are readable/writable.
 * If file1 / file2 does not exist or is unreadable / unwritable,
 * an error message is printed and the program exits with a status of 1.
 *
 * @param file1 The path to the first file.
 * @param file2 The path to the second file.
 */

// plsfix need changes to check for directories

void	check_files(char *file1, char *file2)
{
	int	file2_fd;

	if (access(file1, F_OK) == -1 || access(file1, R_OK) == -1)
	{
		perror("Error: File 1 does not exist or unreadable\n");
		exit(1);
	}
	file2_fd = open(file1, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (file2_fd == -1)
	{
		perror("Error: File2 creation unsuccesful\n");
		exit(1);
	}
	if (access(file2, F_OK) == -1 || access(file2, W_OK) == -1)
	{
		perror("Error: File 2 does not exist or unwritable\n");
		exit(1);
	}
	close(file2_fd);
	return ;
}

/**
 * Executes a child process to handle a command in a pipeline.
 *
 * close unused pipe file descriptors
 * use dup2 to redirect the output of the command to the pipe
 * 		done by duplicating write end of the pipe to STDOUT_FILENO 
 * as the write end of the pipe is now duplicated to STDOUT_FILENO,
 * 	close the write end of the pipe
 * open file1 for reading
 * use dup2 to redirect the input of the command to file1
 * 		done by duplicating file1 to STDIN_FILENO
 * execute the command
 * Handle failures and errors till this point
 * exit with status of 0 if successful
 * 
 * @param cmd The command to be executed.
 * @param pipe_fds An array of pipe file descriptors.
 * @param file1 The input file for the command.
 * @param envp The environment variables for the command.
 */

void	child_process(char *cmd, int *pipe_fds, char *file1, char **envp)
{
	int	file1_fd;

	close(pipe_fds[0]);
	if (dup2(pipe_fds[1], STDOUT_FILENO) == -1)
		return (perror("Error: cmd1 dup2 fd-out\n"), exit(EXIT_FAILURE));
	close(pipe_fds[1]);
	file1_fd = open(file1, O_RDONLY);
	if (file1_fd == -1)
		return (perror("Error: file1 doesn't open\n"), exit(EXIT_FAILURE));
	if (dup2(file1_fd, STDIN_FILENO) == -1)
		return (perror("Error: cmd1 dup2 fd-in\n"), exit(EXIT_FAILURE));
	exec_cmd(cmd, envp);
}

/**
 * Executes the parent process in the pipex program.
 * 
 * @param cmd The command to be executed.
 * @param pipe_fds The file descriptors for the pipe.
 * @param file2 The name of the output file.
 * @param envp The environment variables.
 */

void	parent_process(char *cmd, int *pipe_fds, char *file2, char **envp)
{
	int	file2_fd;

	close(pipe_fds[1]);
	if (dup2(pipe_fds[0], STDIN_FILENO) == -1)
		return (perror("Error: Dup2 failed\n"), exit(EXIT_FAILURE));
	close(pipe_fds[0]);
	file2_fd = open(file2, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (file2_fd == -1)
		return (perror("Error: Open failed for file2\n"), exit(EXIT_FAILURE));
	if (dup2(file2_fd, STDOUT_FILENO) == -1)
		return (perror("Error: Dup2 failed\n"), exit(EXIT_FAILURE));
	close(file2_fd);
	exec_cmd(cmd, envp);
}

/**
 * @file pipex.c
 * @brief This file contains the main function for the pipex program.
 *
 * pipex program takes 4 arguments: file1, cmd1, cmd2, file2.
 * creates a pipe
 * forks child process,
 * redirects input & output of child process to file1 and file2 respectively.
 * The child process executes cmd1, and the parent process executes cmd2.
 * The output of cmd1 is piped to the input of cmd2.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of strings containing the command-line arguments.
 * @param envp An array of strings containing the environment variables.
 * @return EXIT_SUCCESS if executed successfully, EXIT_FAILURE  otherwise.
 */

int	main(int argc, char **argv, char **envp)
{
	char	*file1;
	char	*file2;
	int		pipe_fds[2];
	pid_t	child_pid;

	if (argc != 5)
		return (perror("Error: Incorrect num of  arguments\n"), EXIT_FAILURE);
	file1 = argv[1];
	file2 = argv[4];
	check_files(file1, file2);
	if (pipe(pipe_fds) == -1)
		return (perror("Error: Pipe failed\n"), EXIT_FAILURE);
	child_pid = fork();
	if (child_pid == -1)
		return (perror("Error: Fork failed\n"), EXIT_FAILURE);
	if (child_pid == 0)
		child_process(argv[2], pipe_fds, file1, envp);
	else
		parent_process(argv[3], pipe_fds, file2, envp);
	return (EXIT_SUCCESS);
}
