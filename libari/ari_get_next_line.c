#include "libari.h"

char	*ari_get_next_line(int fd)
{
	static t_g_list	*head;
	t_g_list		*fd_lst;

	fd_lst = 0;
	if (fd < 0 || BUFFER_SIZE <= 0)
		return (0);

	if (!get_g_list(&head, &fd_lst, fd))
		return (0);

	if (!read_str(&fd_lst, fd))
	{
		if (free_lst(&head, &fd_lst))
			return (NULL);
	}

	return (ret_str(&head, &fd_lst));
}

t_success	join_str(t_g_list **fd_lst, char **str, ssize_t read_len)
{
	char		*temp;
	ssize_t		i;

	i = 0;
	temp = (char *)malloc((*fd_lst)->buf_len + read_len);
	if (!temp)
		return (ERROR);
	memcpy(temp, (*fd_lst)->buf, (*fd_lst)->buf_len);
	memcpy(temp + (*fd_lst)->buf_len, *str, read_len);
	free(*str);
	free((*fd_lst)->buf);
	(*fd_lst)->buf_len = (*fd_lst)->buf_len + read_len;
	(*fd_lst)->buf = temp;
	while (i < (*fd_lst)->buf_len)
	{
		if (temp[i] == '\n')
			return (SUCCESS);
		i++;
	}
	return (CONTINUE);
}

t_success	read_str(t_g_list **fd_lst, int fd)
{
	char		*str;
	ssize_t		read_len;
	t_success	state;

	str = (char *)malloc(BUFFER_SIZE);
	if (!str)
		return (ERROR);
	read_len = read(fd, str, BUFFER_SIZE);
	if (read_len < 0 || (read_len == 0 && (*fd_lst)->buf_len == 0))
	{
		free(str);
		return (ERROR);
	}
	if (read_len == 0)
	{
		free(str);
		return (CONTINUE);
	}
	state = join_str(fd_lst, &str, read_len);
	if (state == SUCCESS)
		return (state);
	if (state == CONTINUE)
		return (read_str(fd_lst, fd));
	return (ERROR);
}

void	set_fd_list(t_g_list **fd_lst, t_g_list *prev, int fd)
{
	(*fd_lst)->fd = fd;
	(*fd_lst)->prev = prev;
	(*fd_lst)->next = NULL;
	(*fd_lst)->buf_len = 0;
	(*fd_lst)->buf = 0;
}

ssize_t	get_ret_len(t_g_list **fd_lst)
{
	ssize_t	ret_len;

	ret_len = 0;
	while (ret_len < (*fd_lst)->buf_len)
	{
		if ((*fd_lst)->buf[ret_len++] == '\n')
			break ;
	}
	return (ret_len);
}

char	*set_save(t_g_list **fd_lst, char **ret, ssize_t ret_len)
{
	char	*save;

	save = (char *)malloc((*fd_lst)->buf_len - ret_len);
	if (!save)
	{
		free(*ret);
		return (NULL);
	}
	memcpy(save, (*fd_lst)->buf + ret_len, (*fd_lst)->buf_len - ret_len);
	free((*fd_lst)->buf);
	(*fd_lst)->buf_len = (*fd_lst)->buf_len - ret_len;
	(*fd_lst)->buf = save;
	return (save);
}

char	*ret_str(t_g_list **head, t_g_list **fd_lst)
{
	char	*ret;
	char	*save;
	ssize_t	ret_len;

	ret_len = get_ret_len(fd_lst);
	ret = (char *)malloc(ret_len + 1);
	if (!ret)
	{
		if (free_lst(head, fd_lst))
			return (NULL);
	}
	memcpy(ret, (*fd_lst)->buf, ret_len);
	ret[ret_len] = '\0';
	if ((*fd_lst)->buf_len - ret_len)
	{
		save = set_save(fd_lst, &ret, ret_len);
		if (!save)
			return (NULL);
	}
	else
		free_lst(head, fd_lst);
	return (ret);
}

t_success	free_lst(t_g_list **head, t_g_list **fd_lst)
{
	t_g_list	*fd_prev;
	t_g_list	*fd_next;

	fd_prev = (*fd_lst)->prev;
	fd_next = (*fd_lst)->next;
	if ((*fd_lst)->buf_len != 0)
		free((*fd_lst)->buf);
	if ((*fd_lst)->fd == (*head)->fd)
		*head = fd_next;
	if (fd_prev)
		fd_prev->next = fd_next;
	if (fd_next)
		fd_next->prev = fd_prev;
	free(*fd_lst);
	if (!fd_prev || !fd_next)
		*fd_lst = 0;
	return (SUCCESS);
}

t_success	get_g_list(t_g_list **head, t_g_list **fd_lst, int fd)
{
	t_g_list	*prev;

	prev = NULL;
	if (*head)
	{
		*fd_lst = *head;
		while (*fd_lst && (*fd_lst)->fd != fd)
		{
			prev = *fd_lst;
			*fd_lst = (*fd_lst)->next;
		}
		if (*fd_lst && (*fd_lst)->fd == fd)
			return (CONTINUE);
	}
	*fd_lst = (t_g_list *)malloc(sizeof(t_g_list));
	if (!(*fd_lst))
		return (ERROR);
	set_fd_list(fd_lst, prev, fd);
	if (prev)
		prev->next = *fd_lst;
	else
		*head = *fd_lst;
	return (SUCCESS);
}
