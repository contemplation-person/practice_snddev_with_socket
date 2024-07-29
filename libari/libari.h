#ifndef LIBARI
# define LIBARI

# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/types.h>
# include <string.h>

# define ASCII_COLOR_RED "\x1b[1;31m"
# define ASCII_COLOR_GREEN "\x1b[1;32m"
# define ASCII_COLOR_YELLOW "\x1b[1;33m"
# define ASCII_COLOR_BLUE "\x1b[1;34m"
# define ASCII_COLOR_MAGENTA "\x1b[1;35m"
# define ASCII_COLOR_CYAN "\x1b[1;36m"
# define ASCII_COLOR_RESET "\x1b[0m"

# define COLOR_RED_CODE 1
# define COLOR_GREEN_CODE 2
# define COLOR_YELLOW_CODE 3
# define COLOR_BLUE_CODE 4
# define COLOR_MAGENTA_CODE 5
# define COLOR_CYAN_CODE 6
# define COLOR_RESET_CODE 7

//======================get_next_line.src==============

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 2048
# endif

typedef enum e_success
{
	ERROR = 0,
	SUCCESS,
	CONTINUE
}t_success;

typedef struct s_gnl_list
{
	ssize_t				fd;
	ssize_t				buf_len;
	char				*buf;
	struct s_gnl_list	*next;
	struct s_gnl_list	*prev;
}t_g_list;


t_success	join_str(t_g_list **fd_lst, char **str, ssize_t read_len);
t_success	read_str(t_g_list **fd_lst, int fd);
char		*set_save(t_g_list **fd_lst, char **ret, ssize_t ret_len);
ssize_t		get_ret_len(t_g_list **fd_lst);
char		*ret_str(t_g_list **head, t_g_list **fd_lst);

t_success	free_lst(t_g_list **head, t_g_list **fd_lst);
t_success	get_g_list(t_g_list **head, t_g_list **fd_lst, int fd);
void		set_fd_list(t_g_list **fd_lst, t_g_list *prev, int fd);
//===================================================


// main proto
char    *ari_get_next_line(int fd);
size_t  ari_putstr_fd(const char *s, int fd);
size_t  ari_putendl_fd(const char *s, int fd);
void    ari_title_print(const char *string, const int color_code); 
void    ari_title_print_fd(const int fd, const char *string, const int color_code);
void    ari_print_error(const char *string, const char *file, const int line);

#endif
