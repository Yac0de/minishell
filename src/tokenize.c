/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymeziane <ymeziane@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/31 15:23:42 by ymeziane          #+#    #+#             */
/*   Updated: 2024/02/05 15:19:06 by ymeziane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

//Creates a new token, adds a duplicate of element and returns it.
t_token	*create_new_token(char *element)
{
	t_token	*token;

	token = (t_token *)malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->element = ft_strdup(element);
	token->next = NULL;
	return (token);
}

//Returns the size of the element between quotes. 
//Handles " and ' quotes.
int	compute_quotes_size(char *line)
{
	int	i;

	i = 0;
	if(line[i] == '\'')
	{
		i++;
		while(line[i] && line[i] != '\'')
			i++;
		i++;
	}
	else if(line[i] == '"')
	{
		i++;
		while(line[i] && line[i] != '"')
			i++;
		i++;
	}
	return (i);
}

size_t compute_len(char *element)
{
	size_t len;
	int flag_double_quotes;
	int flag_single_quotes;

	len = 0;
	flag_double_quotes = 0;
	flag_single_quotes = 0;
	while (element[len])
	{
		if(element[len] == ' ' && flag_double_quotes % 2 == 0 && flag_single_quotes % 2 == 0)
			break;
		if (element[len] == '"' && flag_single_quotes % 2 == 0)
			flag_double_quotes++;
		if (element[len] == '\'' && flag_double_quotes % 2 == 0)
			flag_single_quotes++;
		len++;
	}
	return (len);
}

//Takes the pointer to line and extracts and returns the first element  it encounters.
//We first calculate the lenght of the element and then duplicate it. If the element starts with 
//quote we go until the next quote.
char	*get_element(char *line)
{
	char	*element;
	int	i;
	int	j;

	i = 0;
	j = 0;
	
	i = compute_len(line);
	element = malloc(i + 1);
	if (!element)
		return (NULL);
	while (j < i)
	{
		element[j] = line[j];
		j++;
	}
	element[j] = '\0';
	return (element);
}

//This function will take the string line and return a duplicate where every occurence of a variable is changed into its assignation.
//A variable starts with a $ and end with anything that is not 
//an alphadigit char or a _.
char	*replace_in_line(char *line)
{
	printf("Line at the beginning = %s\n");
	char	*var;
	char	*newline;
	int	varlen;
	int	linelen;
	int	i;

	i = 0;
	linelen = 0;
	varlen = 0;
	while (line[i])
	{
		if (line[i] == '$')
		{
			while (ft_isdigit(line[i]) || ft_isalpha(line[i]) || line[i] == '_')
			{
				i++;
				varlen++;
			}
		}
		i++;
	}
	var = malloc(sizeof(char) * (varlen + 1));
	var = ft_strndup(ft_strchr("$", varlen));
	while (ft_strncmp(var, vartab, varlen));
		vartab++;
	linelen = ft_strlen(line) - varlen + ft_strlen(vartab + (varlen + 1));	
	newline = malloc(sizeof(char) * linelen + 1);
	// reste a copier dans newline.
	printf("Line at the end = %s\n");
}

//Creates and adds a new token to the end of tokenlist. Returns the pointer to line incremented by the len of the element. to the end of the element. If the element is a var ($VAR) it will replace the variable by its value. char	*add_token(char	*line, t_token **tokenlist) {
	t_token	*tmp;
	char	*element;
	int	elementlen;

	element = get_element(line);
	elementlen = ft_strlen(element);
	if (has_a_variable(element))
		return (replace_in_line(line));
	if (!(*tokenlist))
	{
		*tokenlist = create_new_token(element);
		return (line + elementlen);
	}
	tmp = *tokenlist;
	while (tmp->next)
		tmp = tmp->next;
	if (has_a_variable(element))
		return (replace_in_line(line));
	tmp->next = create_new_token(element);
	return (line + elementlen);
}

size_t count_del_quotes(char *element)
{
	size_t single_quote;
	size_t double_quote;

	single_quote = 0;
	double_quote = 0;
	while (*element)
	{
		if(*element == '\'' && double_quote % 2 == 0)
			single_quote++;
		if(*element == '"' && single_quote % 2 == 0)
			double_quote++;
		element++;
	}
	if(single_quote % 2 != 0 || double_quote % 2 != 0)
	{
		printf("Error: Unmatched quotes\n");
		exit(EXIT_FAILURE);
	}
	return (single_quote + double_quote);
}

char	*clean_up_quotes(char *element)
{
	int i = 0;
	int j = 0;
	size_t quotes_count = count_del_quotes(element);
	char *new_element;
	new_element = malloc(ft_strlen(element) - quotes_count + 1);
	if(!new_element)
		return (NULL);
	while (element[i])
	{
		if(element[i] == '\'')
		{
			i++;
			while(element[j] && element[i] != '\'')
				new_element[j++] = element[i++];
		}
		else if(element[i] == '"')
		{
			i++;
			while(element[j] && element[i] != '"')
				new_element[j++] = element[i++];
		}
		else
			new_element[j++] = element[i];
		i++;
	}
	new_element[j] = '\0';
	return (new_element);
}

void	clean_up_tokens(t_token **tokenlist)
{
	t_token	*tmp;

	tmp = *tokenlist;
	while (tmp)
	{
		if(ft_strcmp(tmp->element, "\"\"")	== 0)
			return;
		tmp->element = clean_up_quotes(tmp->element);
		tmp = tmp->next;
	}
}

//Returns a pointer to the first element of the linked list of all the tokens in 
//the bash expression.
//You go trough the line, skip whitespaces, and adds a token every time it encounters
//an element of a bash expression. The add token while increment the line pointer 
//to the end of the element so you can then continue trough the rest of the line.
t_token	**tokenize(char	*line)
{
	t_token	**tokenlist;

	tokenlist = malloc(sizeof(t_token*));
	*tokenlist = NULL;
	while (*line)
	{
		if ((*line < '\t' || *line > '\r') && *line != ' ')
			line = add_token(line, tokenlist);
		else
			line++;
	}
	clean_up_tokens(tokenlist);
	return(tokenlist);
}
