/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_expression.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymeziane <ymeziane@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/28 12:18:48 by ymeziane          #+#    #+#             */
/*   Updated: 2024/04/02 12:28:40 by ymeziane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	exec_command(t_data **data, char **expression, char **env)
{
	char	*path_cmd;

	path_cmd = ft_strdup((*data)->path_cmd);
	free_data_struct(*data);
	execve(path_cmd, expression, env);
	g_status = 1;
	exit(g_status);
}

static void	process_invalid(t_token **tokenlist, t_data **data,
		char **expression, char **args)
{
	print_not_found(expression[0], NULL);
	free_after_execution(tokenlist, data, args, expression);
	exit(g_status);
}

static bool	is_minishell(char *cmd)
{
	if (!cmd || ft_strlen(cmd) < ft_strlen("/minishell"))
		return (false);
	if (access(cmd, X_OK) == 0 && ft_strcmp(cmd + (ft_strlen(cmd)
				- ft_strlen("/minishell")), "/minishell") == 0)
		return (true);
	return (false);
}

/* Here we are almost to the point where we actually launch the command.
We have our line like ls | cat | sort ,
we need to cut it to keep only one command according to the index
Then we configure the output and input according to possible pipes
and we check if the command is a built-in, a command or a invalid command. */
static void	execute_child_process(t_token **tokenlist, t_data **data, int index,
		char **args)
{
	char	**expression;

	expression = cut_arrays_into_expression(args, index);
	free((*data)->path_cmd);
	(*data)->path_cmd = get_path_cmd((*data)->bin_paths, expression[0]);
	if (configure_io(tokenlist, index, data))
	{
		if (type(expression[0], (*data)->env) == BUILTIN)
			exec_builtins(tokenlist, data, expression, args);
		else if (type(expression[0], (*data)->env) == COMMAND)
			exec_command(data, expression, env_list_to_array((*data)->env));
		else
			process_invalid(tokenlist, data, expression, args);
	}
	else
	{
		close_all_pipes(tokenlist, (*data)->pipe_fds, (*data)->nb_pipe);
		free_after_execution(tokenlist, data, args, expression);
	}
	g_status = 1;
}

/* This is the function we use to execute each expression.
First we check if the expression is a single built-in
(if this is the case, we don't execute it in a forked process).
If it's not the case, we fork the process
and execute the command in it by calling the execute_child_process function. */
pid_t	exec_expression(t_token **tokenlist, t_data **data, int index,
		char **args)
{
	pid_t	pid;
	pid_t	pid_right;

	if (check_and_exec_single_builtin(tokenlist, data, args))
		return (0);
	pid = fork();
	if ((*data)->nb_pipe == index)
		pid_right = pid;
	if (pid == 0)
	{
		execute_child_process(tokenlist, data, index, args);
		exit(g_status);
	}
	else if (pid > 0 && is_minishell(args[0]))
		signal(SIGINT, SIG_IGN);
	return (pid_right);
}
