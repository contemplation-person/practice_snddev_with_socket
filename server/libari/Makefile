GREEN			=	\e[0;32m
RESET_COLOR		=	\e[0;37m

CC 				= 	gcc
CPPFLAGS		= 	-Wall -Wextra -Werror

NAME			= 	libari.a	
SRC 			= 	ari_get_next_line.c \
					ari_putendl_fd.c \
					ari_putstr_fd.c \
					ari_title_print.c

OBJS	  		= 	$(SRC:.c=.o)
				
AR				= 	ar
ARFLAG			= 	rsc

.PHONY: all clean fclean re 

$(NAME) : $(OBJS)
	$(AR) $(ARFLAG) $@ $^
	echo "\n"

all: $(NAME)

clean:
	@rm -f $(OBJS)

fclean : clean
	@rm -f $(NAME)

re : 
	@make fclean 
	@make all
