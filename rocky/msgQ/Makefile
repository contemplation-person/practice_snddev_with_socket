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

NAME 				:= queue_client

SRCS 				:= queue_client.c 
OBJS 				:= $(SRCS:%.c=%.o)

RM 					:= rm -rf

INCLUDE 			:= -Ilibari -Iincludes 
LIB_DIR 			:= -lari 
LIB_PATH 			:= -Llibari

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

re: fclean
	@$(MAKE) all

CFLAGS += -g3

