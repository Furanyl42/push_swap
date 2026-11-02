# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/10/19 17:10:39 by nide-wil          #+#    #+#              #
#    Updated: 2026/11/02 12:00:00 by nide-wil         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= push_swap

CC			= cc
CFLAGS		= -Wall -Wextra -Werror

LIBFT_DIR	= libft
LIBFT		= $(LIBFT_DIR)/libft.a
INC			= -I. -I$(LIBFT_DIR)

SRC			= main.c \
			  parsing.c \
			  parsing_utils.c \
			  parsing_fill.c \
			  stack.c \
			  disorder.c \
			  utils.c \
			  bench.c \
			  bench_utils.c \
			  ops_swap.c \
			  ops_push.c \
			  ops_rotate.c \
			  ops_reverse.c \
			  sort_dispatcher.c \
			  sort_simple.c \
			  sort_medium.c \
			  sort_complex.c \
			  complex_presort.c \
			  complex_move.c \
			  complex_insert.c \
			  complex_pull.c \
			  array_utils.c \
			  sorts_utils.c

OBJ			= $(SRC:.c=.o)

CHECKER			= checker
CHECKER_SRC		= checker_main.c \
				  checker_parse.c \
				  checker_apply.c \
				  checker_read.c
CHECKER_OBJ		= $(CHECKER_SRC:.c=.o)
CHECKER_SHARED	= parsing_utils.o \
				  stack.o \
				  disorder.o \
				  utils.o \
				  ops_swap.o \
				  ops_push.o \
				  ops_rotate.o \
				  ops_reverse.o

all: $(NAME)

bonus: $(CHECKER)

$(LIBFT):
	@make -C $(LIBFT_DIR)

$(NAME): $(OBJ) $(LIBFT)
	$(CC) $(CFLAGS) $(OBJ) -L$(LIBFT_DIR) -lft -o $(NAME)

$(CHECKER): $(CHECKER_OBJ) $(CHECKER_SHARED) $(LIBFT)
	$(CC) $(CFLAGS) $(CHECKER_OBJ) $(CHECKER_SHARED) \
		-L$(LIBFT_DIR) -lft -o $(CHECKER)

%.o: %.c push_swap.h
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
	rm -f $(OBJ) $(CHECKER_OBJ)
	@make clean -C $(LIBFT_DIR)

fclean: clean
	rm -f $(NAME) $(CHECKER)
	@make fclean -C $(LIBFT_DIR)

re: fclean all

.PHONY: all bonus clean fclean re
