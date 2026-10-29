# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/10/19 17:10:39 by nide-wil          #+#    #+#              #
#    Updated: 2026/10/29 17:40:33 by nide-wil         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME        = push_swap

CC          = cc
CFLAGS      = -Wall -Wextra -Werror

LIBFT_DIR   = libft
LIBFT       = $(LIBFT_DIR)/libft.a
LIBFT_INC   = -I$(LIBFT_DIR)
LIBFT_LIB   = -L$(LIBFT_DIR) -lft

SRC         = main.c \
              ops.c \
              parsing.c \
              sort_small.c \
              stack.c \
              generator.c \
              disorder.c \
              sort_dispatcher.c \
              turk/cost.c \
              turk/move_utils.c \
              turk/phase1.c \
              turk/phase2.c \
              turk/turk.c

OBJ         = $(SRC:.c=.o)

TEST_SRC    = $(SRC)
TEST_OBJ    = $(OBJ)
TEST_NAME   = test

INC         = -I. $(LIBFT_INC)

all: $(LIBFT) $(NAME)

$(LIBFT):
	@make -C $(LIBFT_DIR)

$(NAME): $(OBJ) $(LIBFT)
	$(CC) $(CFLAGS) $(OBJ) $(LIBFT_LIB) -o $(NAME)

%.o: %.c push_swap.h
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

test: $(LIBFT) $(TEST_OBJ)
	$(CC) $(CFLAGS) $(TEST_OBJ) $(LIBFT_LIB) -o $(TEST_NAME)
	@./$(TEST_NAME)

clean:
	rm -f $(OBJ) turk/*.o
	@make clean -C $(LIBFT_DIR)

fclean: clean
	rm -f $(NAME) $(TEST_NAME)
	@make fclean -C $(LIBFT_DIR)

re: fclean all

.PHONY: all clean fclean re
