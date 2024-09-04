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

CC			  	:=	gcc
CFLAGS		  	:=	-Wall -Wextra -Werror

NAME			:=	snd_client

SRCS_DIR		:=	srcs
SRC				:=	main.c 
SRCS			:=	$(addprefix $(SRCS_DIR)/, $(SRC))

OBJ_DIR			:=	objs
OBJS			:=	$(SRC:%.c=$(OBJ_DIR)/%.o)

RM				:=	rm -rf
INCLUDE			:=	-Ilibari -Iincludes

LIB_PATH		:=	$(PWD)/libari
LIB_DIR			:=	-ljson-c 

.PHONY:all clean fclean re test

$(OBJ_DIR)/%.o:		$(SRCS)
					@mkdir -p $(@D)
					@$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

$(NAME):			$(OBJS)
					@$(MAKE) -C $(LIB_PATH)
					$(CC) $(CFLAGS) $(INCLUDE) $(OBJS) $(LIB_DIR) libari/libari.a -o $(NAME) 

all:				$(NAME)

clean:
					$(RM) $(OBJS)
					@$(MAKE) -C $(LIB_PATH) clean

fclean:				clean
					@$(MAKE) -C $(LIB_PATH) fclean
					$(RM) $(NAME)

re:					fclean
					@$(MAKE) all

CFLAGS				+= -g3 # -fsanitize=address

test: $(NAME)
	clear && ./$(NAME)
	
