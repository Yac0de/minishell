#ifndef MINISHELL_H
# define MINISHELL_H

# include "../libft/inc/libft.h"
# include <stdio.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <errno.h>

typedef enum				e_type
{
	COMMAND,
	META_CHAR,
	BUILTIN
}	t_type;

typedef enum				e_exportcases
{
	VAR_INVALID,
	VAR_DEFINED,
	VAR_UNDEFINED
}	t_exportcases;

typedef struct s_token
{
	char			*element;
	int				ttype;
	struct s_token	*next;
}					t_token;

typedef struct s_data
{
	char				*old_pwd;
	char				*home_dir;
	struct s_env_list	**env;
	struct s_env_list	**exp_list;
}					t_data;

typedef struct s_env_list
{
	char				*variable;
	struct s_env_list	*next;
}					t_env_list;

//1. start with the listening loop that will read the user-input from the terminal.
//2. scan the user-input and cut it into tokens. for instance: "cat file.txt" would give 2 tokens: "cat" -> COMMAND and "file.txt" -> FILENAME.
//Save data on each token in a struct.
//3. Start executing the user-input by going trough each token one by one.

//	TOKENIZE
t_token				**tokenize(char *line, t_env_list**env);
char				*add_token(char *line, t_token **tokenlist, t_env_list **env);
char				*get_element(char *line);
t_token				*create_new_token(char *element);
int					has_a_variable(char *element);


//	ENV
t_env_list				**dup_env(char **env);
char	**find_bin_paths(t_env_list **env);
char	*get_env(char *variable, t_env_list **env);
char **env_list_to_array(t_env_list **env);

//	PARSING
void	set_token_types(t_token **tokenlist, t_env_list **env);

//	LOOP
void	listening_loop(t_data **data);

//	HANDLE_ENV_VARS
char	*replace_in_line(char *line, t_env_list **env);

//EXPORT
void	export(char **args, t_env_list **env, t_env_list **exp_list);
t_env_list	**get_export_variables(t_env_list **env);

//	BUILTINS
void	unset(char **args, t_env_list **env, t_env_list **exp_list);
char	*get_current_dir(void);
void	pwd(void);
void	cd(char *absolute_path, t_data **data);
void	env(t_env_list **env);
void	echo(t_token **tokenlist);
void    exit_bash(char *status);

//LST UTILS
void	lst_add_back(t_env_list **lst, t_env_list *new);
t_env_list	*lst_last(t_env_list *lst);
t_env_list	*lst_new(char	*variable);
void	lst_del_one(t_env_list **lst, char *variable, bool export);
int	lst_size(t_env_list **lst);

// EXECUTION
void process_tokens(t_token **tokenlist, t_data **data);

#endif
