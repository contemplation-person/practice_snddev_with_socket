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

# JSON_C_INCLUDE_PATH := /root/json-c-build/json-c

# CC				:=	gcc
# CFLAGS		  	:=	-Wall -Wextra -Werror

# NAME			:=	snd_server

# SRCS			:=	main.c 
# OBJS			:=	$(SRCS:%.c=%.o)

# RM				:=	rm -rf
# INCLUDE			:=	-Ilibari -Iincludes -I$(JSON_C_INCLUDE_PATH)

# LIB_PATH		:=	-L$(PWD)/libari -L$(JSON_C_INCLUDE_PATH)
# LIB_DIR			:=	-ljson-c 

# .PHONY:all clean fclean re test

# %.o: $(SRCS)
# 	$(CC) $(CFLAGS) $(INCLUDE) -c $<

# $(NAME): $(OBJS)
# 	# $(MAKE) -C $(LIB_PATH)
# 	# $(CC) $(CFLAGS) $(INCLUDE) $(OBJS) $(LIB_DIR) libari/libari.a -o $(NAME) 

# all: $(NAME)

# clean:
# 	$(RM) $(OBJS)
# 	@$(MAKE) -C $(LIB_PATH) clean

# fclean:	clean
# 	@$(MAKE) -C $(LIB_PATH) fclean
# 	$(RM) $(NAME)
# 	$(RM) $(TEST_DIR)

# re:	fclean
# 	@$(MAKE) all

# CFLAGS				+= -g3 # -fsanitize=address


JSON_C_INCLUDE_PATH := /root/json-c-build/json-c

CC := gcc
CFLAGS := -Wall -Wextra -Werror

NAME := snd_server

SRCS := main.c 
OBJS := $(SRCS:%.c=%.o)

RM := rm -rf

INCLUDE := -Ilibari -Iincludes -I$(JSON_C_INCLUDE_PATH)
LIB_DIR := -ljson-c -lari
LIB_PATH := -Llibari

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
