# **************************************************************************** #
#																			  #
#														 :::	  ::::::::	#
#	Makefile										   :+:	  :+:	:+:	#
#													 +:+ +:+		 +:+	  #
#	By: juha <jy.h4456@arielnetworks.co.kr>		+#+  +:+	   +#+		 #
#												 +#+#+#+#+#+   +#+			#
#													  #+#	#+#			  #
#													 ###   ########seoul.kr   #
#																			  #
# **************************************************************************** #

CC 					:= gcc
CFLAGS 				:= -Wall -Wextra -Werror  

NAME 				:= snd_server

SRCS 				:= main.c \
					   db.c\
					   json_utils.c\
					   socket_utils.c\
					   utils.c\
					   

OBJS 				:= $(SRCS:%.c=%.o)

RM 					:= rm -rf

INCLUDE 			:= -Ilibari -Iincludes -Ijson-c
LIB_DIR 			:= -ljson-c -lari 
LIB_PATH 			:= -Llibari

INCLUDE+=-I${ALTIBASE_HOME}/include
LIB_DIR+=-L${ALTIBASE_HOME}/lib -lapre -lodbccli -lpthread -lm -ldl

.PHONY: all clean fclean re test

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

$(NAME): $(OBJS)
	@$(MAKE) -C libari -j8
	$(CC) $(CFLAGS) $(INCLUDE) $(OBJS) $(LIB_DIR) $(LIB_PATH) -o $(NAME)

all: $(NAME)

clean:
	@$(MAKE) -C libari clean
	@$(RM) $(OBJS)

fclean: clean
	@$(MAKE) -C libari fclean
	@$(RM) $(NAME)
	@$(RM) file_test

re: fclean
	@$(MAKE) all

CFLAGS += -g3
